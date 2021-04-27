// Put all onload AJAX calls here, and event listeners

$(document).ready(function () {
  // On page-load AJAX Example

  $("#table1_div").hide();
  $("#Gpx_view_table").hide();
  $("#nofiles").hide();
  $("#change_name").hide();
  $("#other_data_tb").hide();
  $("#find_pathTB").hide();
  $("#route_info").hide();
  $("#nocomp").hide();
  $("#STORE_DATA").hide();
  $("#all_routes").hide();
  $("#rtfile").hide();
  $("#fileRT").hide();
  $("#file_all_routes").hide();


  $.ajax({
    type: "get", //Request type
    dataType: "json", //Data type - we will use JSON for almost everything
    url: "/dummy1", //The server endpoint we are connecting to
    success: function (data) {
      if (data.length == 0) {
        $("#nofiles").show();
        $("#nofiles_p").append("Currently there are no GPX files.");
      }

      var ok = 0;

      if (data.length > 0) {

        for (var i in data) {
          if (data[i].message != undefined || data[i].db_info != undefined) {
            continue;
          }
          ok = 1;
          $("#table1_div tbody").append(
            "<tr>" +
            '<td><a href="' +
            data[i][0].name +
            '">' +
            data[i][0].name +
            "</a></td>" +
            "<td>" +
            data[i][1].version +
            "</td>" +
            "<td>" +
            data[i][1].creator +
            "</td>" +
            "<td>" +
            data[i][1].numWaypoints +
            "</td>" +
            "<td>" +
            data[i][1].numRoutes +
            "</td>" +
            "<td>" +
            data[i][1].numTracks +
            "</td>" +
            "</tr>"
          );
        }
        let idx = -1;
        let idx2 = -1;

        for (var i in data) {
          if (data[i].message != undefined) {
            idx = i;
            continue;
          }
          if (data[i].db_info != undefined) {
            idx2 = i;
            continue;
          }
          ok = 1;
          console.log(i);
          $("#gpx_view_dd").append(
            '<option value="' +
            data[i][0].name +
            '">' +
            data[i][0].name +
            "</option>"
          );
        }

        if (!ok) {
          $("#nofiles").show();
          $("#nofiles_p").append("Currently there are no GPX files.");

        }
        else {
          $("#table1_div").show();
          $("#STORE_DATA").show();
          $("#nofiles").hide();
        }

        if (idx != -1) {
          if (data[idx].message != "Invalid GPX File.") {
            let y = data[idx].message;
            $("#uploadSuccess").append(y);
            $("#uploadSuccess").fadeOut(4000); /* 4 Seconds */
          } else {
            if (data.length == 1) {
              $("#nofiles").show();
              $("#nofiles_p").append("Currently there are no files.");
              $("#table1_div").hide();
            }
            alert(data[idx].message + " Click Ok to continue");
          }
        }

        if (idx2 != -1) {
          document.getElementById("db_info_div").hidden = false;
          document.getElementById("after_login").hidden = false;
          $("#Database_info").html(data[idx2].db_info);
        }
      }
    },
    fail: function (error) {
      console.log(error);
    },
  });

  $("#gpx_view_dd").on("change", function () {
    var selectVal = $("#gpx_view_dd option:selected").val();
    $("#nocomp").hide();

    $.ajax({
      type: "POST",
      url: "/get_components",
      dataType: "json",
      data: {
        name: selectVal,
      },
      success: function (successResponse) {
        $("#Gpx_view_table tr").remove();
        $("#Gpx_view_table").append(
          '<tr> <th style="text-align: center">Component</th><th style="text-align: center">Name</th><th style="text-align: center">Number of Points</th><th style="text-align: center">Length</th><th style="text-align: center">Loop</th></tr>'
        );
        $("#Gpx_view_table").show();

        if (successResponse[0].length == 0 && successResponse[1].length == 0) {
          $("#nocomp").html(selectVal + " has no Route/Track.");
          $("#nocomp").show();
          $("#Gpx_view_table").hide();
        }

        var k = 1;
        for (var i in successResponse[0]) {
          {
            $("#Gpx_view_table").append(
              "<tr>" +
              "<td>Route" +
              k +
              "</td>" +
              '<td id="Route' +
              k +
              '">' +
              successResponse[0][i].name +
              "</td>" +
              "<td>" +
              successResponse[0][i].numPoints +
              "</td>" +
              "<td>" +
              successResponse[0][i].len +
              "m</td>" +
              "<td>" +
              (successResponse[0][i].loop ? "TRUE" : "FALSE") +
              "</td>" +
              "</tr>"
            );
            k++;
          }
        }

        k = 1;
        for (var i in successResponse[1]) {
          {
            $("#Gpx_view_table").append(
              "<tr>" +
              "<td>Track" +
              k +
              "</td>" +
              '<td id="Track' +
              k +
              '">' +
              successResponse[1][i].name +
              "</td>" +
              "<td>" +
              successResponse[1][i].numPoints +
              "</td>" +
              "<td>" +
              successResponse[1][i].len +
              "m</td>" +
              "<td>" +
              (successResponse[1][i].loop ? "TRUE" : "FALSE") +
              "</td>" +
              "</tr>"
            );
            k++;
          }
        }
      },
      error: function (errorResponse) {
        console.log(errorResponse);
      },
    });
  });

  $("#Select_Component").mouseover(function (event) {
    $.ajax({
      method: "get",
      url: "/dd_comp",
      success: function (data) {
        console.log(data);
        $("#Select_Component option").remove();

        $("#Select_Component").append(
          '<option value="dummy" selected disabled hidden>Select</option>'
        );
        for (var i in data) {
          $("#Select_Component").append(
            '<option value="' + data[i] + '">' + data[i] + "</option>"
          );
        }
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });

  $("#Select_Component").change(function (event) {
    var num1 = $("#Select_Component").val();
    $("#show1").html("Enter The New Name for " + $("#Select_Component").val());

    $("#change_name").show();
    $.ajax({
      method: "post",
      url: "/data1",
      data: JSON.stringify({ num1: num1 }),
      contentType: "application/json",
    });
  });

  $("#change_name").submit(function (event) {
    event.preventDefault();
    var num1 = $("#component_name").val();

    $.ajax({
      method: "post",
      url: "/data_nc",
      data: JSON.stringify({ num1: num1 }),
      contentType: "application/json",
      success: function (data) {
        console.log(data);
        let s = "#" + data.comp;
        $(s).html(num1);
        alert(data.message);
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });

  // otherdata
  $("#Select_Component_OD").mouseover(function (event) {
    $.ajax({
      method: "get",
      url: "/od_comp",
      success: function (data) {
        console.log(data);
        $("#Select_Component_OD option").remove();

        $("#Select_Component_OD").append(
          '<option value="dummy" selected disabled hidden>Select</option>'
        );
        for (var i in data) {
          $("#Select_Component_OD").append(
            '<option value="' + data[i] + '">' + data[i] + "</option>"
          );
        }
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });

  $("#Select_Component_OD").change(function (event) {
    var num1 = $("#Select_Component_OD").val();
    $("#other_data_tb").show();
    $("#other_data_tb tr").remove();
    $.ajax({
      method: "post",
      url: "/data_od",
      data: JSON.stringify({ num1: num1 }),
      contentType: "application/json",
      success: function (data) {
        $("#other_data_tb").append(
          '<tr><th style="text-align: center">Property</th><th style="text-align: center">Value</th></tr>'
        );
        for (var i in data) {
          $("#other_data_tb").append(
            "<tr>" +
            "<td>" +
            data[i].name +
            "</td>" +
            "<td>" +
            data[i].value +
            "</td>" +
            "</tr>"
          );
        }
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });

  $("#find_path").submit(function (event) {
    event.preventDefault();
    var lat1 = $("#Ilat").val();
    var long1 = $("#Ilong").val();
    var lat2 = $("#Flat").val();
    var long2 = $("#Flong").val();
    var tol = $("#tolerance").val();

    $("#find_pathTB tr").remove();

    $.ajax({
      method: "post",
      url: "/findPath",
      data: JSON.stringify({
        lat1: lat1,
        long1: long1,
        lat2: lat2,
        long2: long2,
        tol: tol,
      }),
      contentType: "application/json",
      success: function (data) {
        $("#find_pathTB").append(
          '<tr> <th style="text-align: center">File Name</th><th style="text-align: center">Component</th><th style="text-align: center">Name</th><th style="text-align: center">Number of Points</th><th style="text-align: center">Length</th><th style="text-align: center">Loop</th></tr>'
        );
        $("#find_pathTB").show();

        console.log(data);
        for (var i in data) {
          for (var j in data[i]) {
            {
              $("#find_pathTB").append(
                "<tr>" +
                "<td>" +
                data[i][j][0].name +
                "</td>" +
                "<td>" +
                data[i][j][0].num +
                "</td>" +
                "<td>" +
                data[i][j][1].name +
                "</td>" +
                "<td>" +
                data[i][j][1].numPoints +
                "</td>" +
                "<td>" +
                data[i][j][1].len +
                "m</td>" +
                "<td>" +
                (data[i][j][1].loop ? "TRUE" : "FALSE") +
                "</td>" +
                "</tr>"
              );
            }
          }
        }
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });

  $("#file_input_for_change").submit(function (event) {
    event.preventDefault();
    var name = $("#addName").val();
    var version = $("#addVersion").val();
    var creator = $("#addCreator").val();

    $.ajax({
      method: "post",
      url: "/create_gpx",
      data: JSON.stringify({
        name: name,
        version: version,
        creator: creator,
      }),
      contentType: "application/json",
      success: function (data) {
        window.location.reload();
        alert(data.message);
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });

  $("#addRoute").mouseover(function (event) {
    $.ajax({
      method: "get",
      url: "/getFilesAddRoute",
      success: function (data) {
        console.log(data);
        $("#addRoute option").remove();

        $("#addRoute").append(
          '<option value="dummy" selected disabled hidden>Select</option>'
        );
        for (var i in data) {
          $("#addRoute").append(
            '<option value="' + data[i].name + '">' + data[i].name + "</option>"
          );
        }
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });

  $("#addRoute").change(function (event) {
    var name = $("#addRoute").val();
    $("#route_info").show();
    let s = "Add Route to " + name;
    $("#NametoAddROute").html(s);
    $("#NametoAddROute").show();

    $.ajax({
      method: "post",
      url: "/addRTfile",
      data: JSON.stringify({ name: name }),
      contentType: "application/json",
      success: function (data) { },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });

  $("#addWaypoints").submit(function (event) {
    event.preventDefault();
    var lat1 = $("#addlat").val();
    var long1 = $("#addlong").val();

    $.ajax({
      method: "post",
      url: "/addWP",
      data: JSON.stringify({ lat1: lat1, long1: long1 }),
      contentType: "application/json",
      success: function (data) {
        alert(data.message);
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });

  $("#addRouteinfo").submit(function (event) {
    event.preventDefault();
    var name = $("#addNameRT").val();

    $.ajax({
      method: "post",
      url: "/addRouteALL",
      data: JSON.stringify({ name: name }),
      contentType: "application/json",
      success: function (data) {
        window.location.reload();
        alert(data.message);
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });


  $("#db_credentials").submit(function (event) {
    event.preventDefault();
    var user = $("#add_username").val();
    var host = $("#add_hostname").val();
    var pass = $("#password_db").val();
    var db_name = $("#add_DB_name").val();


    $.ajax({
      method: "post",
      url: "/loginDB",
      data: JSON.stringify({ user: user, pass: pass, db_name: db_name ,host: host}),
      contentType: "application/json",
      success: function (data) {
        let s = 'Logged In to the Database : ' + db_name + ' of User : ' + user;
        document.getElementById("after_login").hidden = false;
        document.getElementById("db_info_div").hidden = false;
        $("#Database_info").html(s);
        loggedIN = true;
        alert(data.message);
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });

  $("#db_logout").submit(function (event) {
    event.preventDefault();

    $.ajax({
      method: "post",
      url: "/logoutDB",
      success: function (data) {
        document.getElementById("add_username").value = '';
        document.getElementById("password_db").value = '';
        document.getElementById("add_DB_name").value = '';

        $("#Database_info").html("");
        document.getElementById("db_info_div").hidden = true;
        document.getElementById("after_login").hidden = true;
        alert(data.message);
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });

  $("#db_store").submit(function (event) {
    event.preventDefault();

    $.ajax({
      method: "post",
      url: "/DBstore",
      success: function (data) {
        alert(data.message);
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });


  $("#db_clear").submit(function (event) {
    event.preventDefault();

    $.ajax({
      method: "post",
      url: "/DBclear",
      success: function (data) {
        alert(data.message);
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });


  $("#db_show").submit(function (event) {
    event.preventDefault();

    $.ajax({
      method: "post",
      url: "/DBshow",
      success: function (data) {
        alert(data.message);
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });

  $("#rt_display_name").submit(function (event) {
    event.preventDefault();

    $.ajax({
      method: "post",
      url: "/allrts_name",
      success: function (data) {
        $("#all_routes").show();
        $("#all_routes tr").remove();

        $("#all_routes").append(
          '<tr><th style="text-align: center">Route ID</th><th style="text-align: center">Route Name</th><th style="text-align: center">Route Length</th><th style="text-align: center">GPX ID</th></tr>'
        );
        for (var i in data) {
          $("#all_routes").append(
            "<tr>" +
            "<td>" +
            data[i].route_id +
            "</td>" +
            "<td>" +
            data[i].route_name +
            "</td>" +
            "<td>" +
            data[i].route_len +
            "</td>" +
            "<td>" +
            data[i].gpx_id +
            "</td>" +
            "</tr>"
          );
        }


      },
      error: function (err) {
        $("#all_routes").hide();
        alert(err.responseJSON.message);
      },
    });
  });


  $("#rt_display_length").submit(function (event) {
    event.preventDefault();

    $.ajax({
      method: "post",
      url: "/allrts_len",
      success: function (data) {
        $("#all_routes").show();
        $("#all_routes tr").remove();

        $("#all_routes").append(
          '<tr><th style="text-align: center">Route ID</th><th style="text-align: center">Route Name</th><th style="text-align: center">Route Length</th><th style="text-align: center">GPX ID</th></tr>'
        );
        for (var i in data) {
          $("#all_routes").append(
            "<tr>" +
            "<td>" +
            data[i].route_id +
            "</td>" +
            "<td>" +
            data[i].route_name +
            "</td>" +
            "<td>" +
            data[i].route_len +
            "</td>" +
            "<td>" +
            data[i].gpx_id +
            "</td>" +
            "</tr>"
          );

        }
      },
      error: function (err) {
        $("#all_routes").hide();
        alert(err.responseJSON.message);
      },
    });
  });



  $("#db_rtfile_dd").mouseover(function (event) {
    $.ajax({
      method: "get",
      url: "/db_rtfile_dd",
      success: function (data) {
        console.log(data);
        $("#db_rtfile_dd option").remove();

        $("#db_rtfile_dd").append(
          '<option value="dummy" selected disabled hidden>Select</option>'
        );
        for (var i in data) {
          $("#db_rtfile_dd").append(
            '<option value="' + data[i].name + '">' + data[i].name + "</option>"
          );
        }
      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });


  $("#db_rtfile_dd").change(function (event) {
    var name = $("#db_rtfile_dd").val();
    let s = "Routes of " + name;
    $("#rtfile").html(s);
    $("#rtfile").show();
    $("#fileRT").show();
    $("#file_all_routes").hide();

    $.ajax({
      method: "post",
      url: "/rt_file",
      data: JSON.stringify({ name: name }),
      contentType: "application/json",
      success: function (data) {

      },
      error: function (err) {
        alert(err.responseJSON.message);
      },
    });
  });


  $("#file_rt_display_name").submit(function (event) {
    event.preventDefault();

    $.ajax({
      method: "post",
      url: "/file_rts_name",
      success: function (data) {
        $("#file_all_routes").show();
        $("#file_all_routes tr").remove();

        $("#file_all_routes").append(
          '<tr><th style="text-align: center">Route ID</th><th style="text-align: center">Route Name</th><th style="text-align: center">Route Length</th><th style="text-align: center">GPX ID</th></tr>'
        );
        for (var i in data) {
          $("#file_all_routes").append(
            "<tr>" +
            "<td>" +
            data[i].route_id +
            "</td>" +
            "<td>" +
            data[i].route_name +
            "</td>" +
            "<td>" +
            data[i].route_len +
            "</td>" +
            "<td>" +
            data[i].gpx_id +
            "</td>" +
            "</tr>"
          );
        }


      },
      error: function (err) {
        $("#file_all_routes").hide();
        alert(err.responseJSON.message);
      },
    });
  });


  $("#file_rt_display_length").submit(function (event) {
    event.preventDefault();

    $.ajax({
      method: "post",
      url: "/file_rts_len",
      success: function (data) {
        $("#file_all_routes").show();
        $("#file_all_routes tr").remove();

        $("#file_all_routes").append(
          '<tr><th style="text-align: center">Route ID</th><th style="text-align: center">Route Name</th><th style="text-align: center">Route Length</th><th style="text-align: center">GPX ID</th></tr>'
        );
        for (var i in data) {
          $("#file_all_routes").append(
            "<tr>" +
            "<td>" +
            data[i].route_id +
            "</td>" +
            "<td>" +
            data[i].route_name +
            "</td>" +
            "<td>" +
            data[i].route_len +
            "</td>" +
            "<td>" +
            data[i].gpx_id +
            "</td>" +
            "</tr>"
          );

        }
      },
      error: function (err) {
        $("#file_all_routes").hide();
        alert(err.responseJSON.message);
      },
    });
  });


  // $("#someform").submit(function (e) {
  //   e.preventDefault();

  //   $.ajax({});
  // });
});

/* Database Fuctionalities */

