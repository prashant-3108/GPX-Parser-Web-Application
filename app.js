"use strict";

// C library API
const ffi = require("ffi-napi");

// Express App (Routes)
const express = require("express");
const app = express();
const path = require("path");
const fileUpload = require("express-fileupload");

app.use(fileUpload());
app.use(express.static(path.join(__dirname + "/uploads")));

// Minimization
const fs = require("fs");
const JavaScriptObfuscator = require("javascript-obfuscator");
const { stringify } = require("querystring");
const { json } = require("express");
const { RSA_NO_PADDING } = require("constants");
const mysql = require("mysql2/promise");
const { type } = require("os");

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

var files_uploaded = new Array();

// Send HTML at root, do not change
app.get("/", function (req, res) {
  res.sendFile(path.join(__dirname + "/public/index.html"));
});

// Send Style, do not change
app.get("/style.css", function (req, res) {
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname + "/public/style.css"));
});

// Send obfuscated JS, do not change
app.get("/index.js", function (req, res) {
  fs.readFile(
    path.join(__dirname + "/public/index.js"),
    "utf8",
    function (err, contents) {
      const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {
        compact: true,
        controlFlowFlattening: true,
      });
      res.contentType("application/javascript");
      res.send(minimizedContents._obfuscatedCode);
    }
  );
});

var uploaded_file = "";

app.post("/upload", function (req, res) {
  if (!req.files) {
    return res.status(400).send("No files were uploaded.");
  }

  let uploadFile = req.files.uploadFile;

  for (var i in files_uploaded) {
    if (files_uploaded[i] == uploadFile.name) {
      return res.status(400).send("<h1>File Already Exists.</h1>");
    }
  }
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv("uploads/" + uploadFile.name, function (err) {
    if (err) {
      return res.status(500).send(err);
    }
    uploaded_file = uploadFile.name;
    return res.redirect("/");
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get("/uploads/:name", function (req, res) {
  fs.stat("uploads/" + req.params.name, function (err, stat) {
    if (err == null) {
      res.sendFile(path.join(__dirname + "/uploads/" + req.params.name));
    } else {
      console.log("Error in file downloading route: " + err);
      res.send("");
    }
  });
});

//******************** code goes here ********************//

app.use(express.urlencoded({ extended: true }));
app.use(express.json());

app.get("/endpoint1", function (req, res) {
  let retStr = req.query.stuff + " " + req.query.junk;
  res.send({
    stuff: retStr,
  });
});

app.listen(portNum);
console.log("Running app at localhost: " + portNum);

const upload_path = path.join(__dirname, "/uploads");
const gpxSchema = path.join(__dirname, "/parser/include/gpx.xsd");

let sharedLib = ffi.Library("./libgpxparser", {
  /* name = return : arguments */

  validate_gpx_file: ["bool", ["string", "string"]],
  gpxfile_to_json: ["string", ["string"]],
  get_all_tracks: ["string", ["string"]],
  get_all_routes: ["string", ["string"]],
  dropdown_list: ["string", ["string"]],
  name_change_gpx: ["bool", ["string", "string", "string", "int"]],
  other_data: ["string", ["string", "string", "int"]],
  find_path: [
    "string",
    ["string", "string", "double", "double", "double", "double", "double"],
  ],
  CreateGPXFile: ["bool", ["string", "string", "string"]],
  ADDrouteToFile: ["bool", ["string", "string", "string", "string"]],
  listWP_Route: ["string", ["string", "int"]],
});

let connection;

// To be used.


let dbConf = {
  host: '',
  user: '',
  password: '',
  database: ''
};


// let dbConf = {
//   host: "localhost",
//   user: "",
//   password: "",
//   database: "",
// };

app.get("/dummy1", function (req, res) {
  var json_list = new Array();
  var messageback = "";
  files_uploaded = [];

  fs.readdir(upload_path, (err, files) => {
    files.forEach((file) => {
      {
        const filename = path.join(__dirname, "/uploads/", file);
        var x = sharedLib.validate_gpx_file(filename, gpxSchema);
        if (x === true) {
          if (uploaded_file == path.basename(filename)) {
            messageback = "Uploaded " + path.basename(filename) + ".";
          }
          let xx = sharedLib.gpxfile_to_json(filename);
          let gpx_details = JSON.parse(xx);

          let name = path.basename(filename);
          files_uploaded.push(name);
          let s = '{"name" : "' + name + '"}';
          let n = JSON.parse(s);

          var jsons = new Array();
          jsons.push(n);
          jsons.push(gpx_details);
          json_list.push(jsons);
        } else {
          if (uploaded_file == path.basename(filename)) {
            messageback = "Invalid GPX File.";
          }
          fs.unlinkSync(filename); /* Delete Invalid Files */
        }
      }
    });
    if (messageback != "") {
      json_list.push({ message: messageback });
    }
    uploaded_file = "";
    let db_loggenin = "";
    if (dbConf.database != "" || dbConf.password != "" || dbConf.user != "") {
      db_loggenin =
        "Logged In to the Database : " +
        dbConf.database +
        " of User : " +
        dbConf.user;
      json_list.push({ db_info: db_loggenin });
    }

    res.send(json_list);
  });
});

var in_gpx_view = "";

app.post("/get_components", function (req, res) {
  var file_name_selected = req.body.name;

  in_gpx_view = file_name_selected;

  fs.readdir(upload_path, (err, files) => {
    files.forEach((file) => {
      const filename = path.join(__dirname, "/uploads/", file);
      if (path.basename(filename) == file_name_selected) {
        let all_routes = sharedLib.get_all_routes(filename);
        let all_tracks = sharedLib.get_all_tracks(filename);

        var json_list = new Array();
        let p1 = JSON.parse(all_routes);
        let p2 = JSON.parse(all_tracks);

        json_list.push(p1);
        json_list.push(p2);

        if (json_list.length > 0) {
          res.json(json_list);
        }
      }
    });
  });
});

app.post("/data", function (req, res) {
  var a = req.body.num1;

  if (in_gpx_view == "") {
    res
      .status(400)
      .json({ message: "Please Select a File in GPX File Panel." });
  }

  if (a == "" || a == undefined) {
    res.status(400).json({ message: "Enter a Valid File Name." });
  }
  var message = "";
  var ok = 0;

  var id = a.replace(/\D/g, "");

  if (id == "") {
    res.status(400).json({ message: "Enter a Valid File Name." });
  }

  fs.readdir(upload_path, (err, files) => {
    files.forEach((file) => {
      if (path.basename(file) == in_gpx_view) {
        const filename = path.join(__dirname, "/uploads/", in_gpx_view);
        let checked = sharedLib.component_validate(filename, a, id);

        if (checked == true) {
          message = "Proceed";
          ok = 1;
        } else if (checked == false) {
          message = "Please Enter Route/Track Within Range.";
        }

        let x = '{"message":"' + message + '"}';
        let y = JSON.parse(x);
        if (ok) {
          res.status(200).send(y);
        } else {
          res.status(400).send(y);
        }
      }
    });
  });
});

app.get("/dd_comp", function (req, res) {
  if (in_gpx_view != "") {
    fs.readdir(upload_path, (err, files) => {
      files.forEach((file) => {
        if (path.basename(file) == in_gpx_view) {
          const filename = path.join(__dirname, "/uploads/", in_gpx_view);
          let s = sharedLib.dropdown_list(filename);
          let j = JSON.parse(s);
          res.json(j);
        }
      });
    });
  }
});

var selected_dd;

app.post("/data1", function (req, res) {
  var a = req.body.num1;
  selected_dd = a;
});

app.post("/data_nc", async function (req, res) {
  var nametochange = req.body.num1;
  var number = selected_dd.replace(/\D/g, "");

  fs.readdir(upload_path, (err, files) => {
    files.forEach((file) => {
      if (path.basename(file) == in_gpx_view) {
        const filename = path.join(__dirname, "/uploads/", in_gpx_view);

        let r1 = sharedLib.get_all_routes(filename);
        let rj1 = JSON.parse(r1);

        let old_name = rj1[number - 1].name;
        let old_length = rj1[number - 1].len;

        let done = sharedLib.name_change_gpx(
          filename,
          nametochange,
          selected_dd,
          number
        );
        if (done == true) {

          /* Add to d database also.. */
          if (selected_dd[0] == 'R') {

            async function change_name() {
              if (dbConf.database == "" || dbConf.password == "" || dbConf.user == "") {
                return res.json({
                  message: "Name Changed Successfully.",
                  comp: selected_dd,
                });
              }
              else {
                /* get gpx item from in_gpx_view and then update the table */
                let query2 =
                  "SELECT gpx_id FROM FILE where file_name = '" +
                  in_gpx_view +
                  "';";

                let x1;

                try {
                  x1 = await connection.execute(query2)
                } catch (e) {
                  console.log("Query error: " + e);
                  return res.status(400).send({
                    message:
                      "Failed to gpx_id of the GPX view File.",
                  });
                }

                let y1 = JSON.stringify(x1[0]);
                let z1 = JSON.parse(y1);


                let query = "UPDATE ROUTE SET route_name = '" + nametochange + "' WHERE route_name = '" + old_name + "' AND gpx_id = " + z1[0].gpx_id + " AND route_len = " + old_length + ";";

                try {
                  x1 = await connection.execute(query)
                } catch (e) {
                  console.log("Query error: " + e);
                  return res.status(400).send({
                    message:
                      "Cannot update the name in Database.",
                  });
                }
              }
              return res.json({
                message: "Name Changed Successfully. Name Updated in Database. Click Ok to continue.",
                comp: selected_dd,
              });
            }
            change_name();

          }
          else {   /* Its a track */
            res.json({
              message: "Name Changed Successfully. Click Ok to continue.",
              comp: selected_dd,
            });
          }


        } else {
          res.status(400).json({ message: "Error Occured." });
        }
      }
    });
  });
});

var selected_od;

app.get("/od_comp", function (req, res) {
  if (in_gpx_view != "") {
    fs.readdir(upload_path, (err, files) => {
      files.forEach((file) => {
        if (path.basename(file) == in_gpx_view) {
          const filename = path.join(__dirname, "/uploads/", in_gpx_view);
          let s = sharedLib.dropdown_list(filename);
          let j = JSON.parse(s);
          res.json(j);
        }
      });
    });
  }
});

app.post("/data_od", function (req, res) {
  var a = req.body.num1;
  selected_od = a;

  var number = selected_od.replace(/\D/g, "");

  if (in_gpx_view != "") {
    fs.readdir(upload_path, (err, files) => {
      files.forEach((file) => {
        if (path.basename(file) == in_gpx_view) {
          const filename = path.join(__dirname, "/uploads/", in_gpx_view);
          let s = sharedLib.other_data(filename, selected_od, number);

          if (s) {
            try {
              let od_list = JSON.parse(s);
              if (od_list.length) {
                res.send(od_list);
              } else {
                res.status(400).send({ message: "No GPX Other data Found" });
              }
            } catch (e) {
              res.status(400).send({ message: "Data Corrupted" }); // error in the above string (in this case, yes)!
            }
          }
        }
      });
    });
  }
});

app.post("/findPath", function (req, res) {
  var l1 = parseFloat(req.body.lat1);
  var lg1 = parseFloat(req.body.long1);
  var l2 = parseFloat(req.body.lat2);
  var lg2 = parseFloat(req.body.long2);
  var t = parseFloat(req.body.tol);

  var notOK = 0;

  if (l1 == NaN || l2 == NaN || lg2 == NaN || lg1 == NaN || t == NaN) {
    res
      .status(400)
      .send({ message: "Please Enter Valid Latitudes and Longitudes" });
    notOK = 1;
  }
  if (
    l1 > 90 ||
    l1 < -90 ||
    l2 > 90 ||
    l2 < -90 ||
    lg1 > 180 ||
    lg1 < -180 ||
    lg2 > 180 ||
    lg2 < -180
  ) {
    res
      .status(400)
      .send({ message: "Please Enter Valid Latitudes and Longitudes" });
    notOK = 1;
  }

  if (notOK === 0) {
    var json_list = new Array();
    fs.readdir(upload_path, (err, files) => {
      files.forEach((file) => {
        const filename = path.join(__dirname, "/uploads/", file);

        let s = sharedLib.find_path(
          filename,
          path.basename(file),
          l1,
          lg1,
          l2,
          lg2,
          t
        );
        if (s) {
          try {
            let od_list = JSON.parse(s);
            if (od_list.length > 0) {
              json_list.push(od_list);
            }
          } catch (e) {
            // error in the above string (in this case, yes)!
          }
        }
      });

      if (json_list.length == 0) {
        res
          .status(400)
          .send({ message: "No Files/Components having this Path Found." });
      } else {
        res.send(json_list);
      }
    });
  }
});

app.post("/create_gpx", function (req, res) {
  var t_n = req.body.name;
  var v = parseFloat(req.body.version);
  var c = req.body.creator;

  var idx = t_n.length;
  for (var i in t_n) {
    if (t_n[i] == ".") {
      idx = i;
      break;
    }
  }

  var n = t_n.slice(0, idx);
  n += ".gpx";

  var present = 0;
  for (var i in files_uploaded) {
    if (files_uploaded[i] == n) {
      present = 1;
      break;
    }
  }
  if (present) {
    res.status(400).send({ message: "File Already Present" });
  }

  if (!present) {
    var json_doc = { version: v, creator: c };
    var jsonSTRING = JSON.stringify(json_doc);

    if (v == NaN) {
      res.status(400).send({ message: "Version must be a Decimal Value" });
    }

    const filename = path.join(__dirname, "/uploads/" + n);

    let s = sharedLib.CreateGPXFile(filename, gpxSchema, jsonSTRING);

    if (s == true) {
      var ss = "Successfully Created " + n + " Click Ok to Continue";
      var j = { message: ss };

      res.json(j);
    } else {
      res.status(400).send({ message: "Failed to Validate/Write GPX File." });
    }
  }
});

// Add Route .......

app.get("/getFilesAddRoute", function (req, res) {
  var names = new Array();
  fs.readdir(upload_path, (err, files) => {
    files.forEach((file) => {
      let nm = '{"name":"' + path.basename(file) + '"}';
      let s = JSON.parse(nm);
      names.push(s);
    });

    res.send(names);
  });
});

var wp_added = new Array();
var RouteFile = "";

app.post("/addRTfile", function (req, res) {
  var t_n = req.body.name;
  RouteFile = t_n;
  if (wp_added.length > 0) {
    wp_added.splice(0, wp_added.length);
  }
});

app.post("/addWP", function (req, res) {
  var l1 = parseFloat(req.body.lat1);
  var lg1 = parseFloat(req.body.long1);

  var notOK = 0;

  if (l1 == NaN || lg1 == NaN) {
    res
      .status(400)
      .send({ message: "Please Enter Valid Latitudes and Longitudes" });
    notOK = 1;
  }
  if (l1 > 90 || l1 < -90 || lg1 > 180 || lg1 < -180) {
    res
      .status(400)
      .send({ message: "Please Enter Valid Latitudes and Longitudes" });
    notOK = 1;
  }

  if (!notOK) {
    let s = '{"lat":' + l1 + ',"lon":' + lg1 + "}";
    let x = JSON.parse(s);
    wp_added.push(x);
    res.send({ message: "Waypoint Stored Successfully." });
  }
});

app.post("/addRouteALL", function (req, res) {
  var rn = req.body.name;

  if (wp_added.length == 0) {
    res.status(400).send({ message: "Add atleast one Waypoint" });
  }

  var nameJS = '{"name":"' + rn + '"}';

  if (RouteFile != "") {
    fs.readdir(upload_path, (err, files) => {
      files.forEach((file) => {
        if (path.basename(file) == RouteFile) {
          const filename = path.join(__dirname, "/uploads/", RouteFile);

          let wpJS = JSON.stringify(wp_added);
          let s = sharedLib.ADDrouteToFile(filename, nameJS, wpJS, gpxSchema);


          /* INSERT Route to Database Also.... */

          /* I would be needing routelisttoJSON and take its last element and insert into routes... */


          /* I need to get Route Id of the inserted Route here */
          /* I would be adding all the points in Wp list to the POINTS TABLE.. */

          if (s) {
            async function addrt() {
              if (dbConf.database == "" || dbConf.password == "" || dbConf.user == "") {
                return res.json({
                  message: "Route Added Successfully Successfully.",
                });
              }
              else {
                let l = sharedLib.get_all_routes(filename);
                let ll = JSON.parse(l);
                let length_of_list = ll.length;

                let query2 =
                  "SELECT gpx_id FROM FILE where file_name = '" +
                  RouteFile +
                  "';";

                let x1;

                try {
                  x1 = await connection.execute(query2)
                } catch (e) {
                  console.log("Query error: " + e);
                  return res.status(400).send({
                    message:
                      "Failed to gpx_id of the GPX view File.",
                  });
                }

                let y1 = JSON.stringify(x1[0]);
                let z1 = JSON.parse(y1);
                let gpxID = z1[0].gpx_id;

                let query =
                  "INSERT INTO ROUTE  (route_name,route_len,gpx_id) VALUES ('" +
                  ll[length_of_list - 1].name +
                  "' ," +
                  ll[length_of_list - 1].len +
                  ", " +
                  gpxID +
                  ");";

                try {
                  await connection.execute(query)
                } catch (e) {
                  console.log("Query error: " + e);
                  return res.status(400).send({
                    message:
                      "Failed to Insert Added route to database.",
                  });
                }

                /* Needing the route_id */

                let query3 =
                  "SELECT route_id FROM ROUTE where gpx_id = " +
                  gpxID +
                  "  AND route_name = '" +
                  ll[length_of_list - 1].name +
                  "' AND route_len = " +
                  ll[length_of_list - 1].len
                ";";

                let x2;
                try {
                  x2 = await connection.execute(query3);
                } catch (e) {
                  console.log("Query error: " + e);
                  return res.status(400).send({
                    message:
                      "Failed to Store the Files. Use Clear Tables to Clean the Suspicious",
                  });
                }

                let y2 = JSON.stringify(x2[0]);
                let z2 = JSON.parse(y2);
                let n = z2.length;
                let rID = (z2[n - 1].route_id);

                for (var j in wp_added) {
                  let query_points =
                    "INSERT INTO POINT  (point_index,latitude,longitude,point_name,route_id) VALUES (" +
                    wp_added[j].index +
                    " ," +
                    wp_added[j].lat +
                    ", " +
                    wp_added[j].lon +
                    ",'" +
                    wp_added[j].name +
                    "', " +
                    rID +
                    ");";

                  try {
                    await connection.execute(query_points);
                  } catch (e) {
                    console.log("Query error: " + e);
                    return res.status(400).send({
                      message:
                        "Failed to Add Points to the POINT table",
                    });
                  }

                }
                return res.send({ message: 'Route Added Successfully. Route Saved to Database ' + dbConf.database });
              }
            }
            addrt();
            wp_added.splice(0, wp_added.length);
          } else {
            wp_added.splice(0, wp_added.length);
            res.status(400).send({
              message: "Failed to Validate. Route Not Added. Please Try Again.",
            });
          }
        }
      });
    });
  }
});

/* Database Code A4 */

app.post("/loginDB", async function (req, res) {
  var uname = req.body.user;
  var hname = req.body.host;
  var pw = req.body.pass;
  var db = req.body.db_name;

  if (dbConf.database != "" || dbConf.password != "" || dbConf.user != "" || dbConf.host != "") {
    res
      .status(400)
      .send({ message: "Logout First to Login To another Database." });
  } else {
    dbConf.user = uname;
    dbConf.password = pw;
    dbConf.database = db;
    dbConf.host = hname;

    try {
      // create the connection
      connection = await mysql.createConnection(dbConf);

      /* Creating Tables */

      await connection.execute(
        "CREATE TABLE IF NOT EXISTS FILE ( gpx_id INT AUTO_INCREMENT, file_name VARCHAR(60) NOT NULL , ver DECIMAL(2,1) NOT NULL , creator VARCHAR(256) NOT NULL , PRIMARY KEY(gpx_id));"
      );
      await connection.execute(
        "CREATE TABLE IF NOT EXISTS ROUTE ( route_id INT AUTO_INCREMENT , route_name VARCHAR(256) , route_len FLOAT(15,7) NOT NULL , gpx_id INT NOT NULL ,PRIMARY KEY(route_id), FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE);"
      );
      await connection.execute(
        "CREATE TABLE IF NOT EXISTS POINT ( point_id INT AUTO_INCREMENT , point_index INT NOT NULL , latitude DECIMAL(11,7) NOT NULL , longitude DECIMAL(11,7) NOT NULL , point_name VARCHAR(256) , route_id INT NOT NULL , PRIMARY KEY(point_id) , FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE);"
      );

      res.send({ message: "Sucessfully Connected to Database." });
    } catch (e) {
      console.log("Query error: " + e);
      res.status(400).send({
        message:
          "Couldn't Connect to Database. Please Re-Enter the Login Credentials.",
      });
      dbConf.user = "";
      dbConf.password = "";
      dbConf.database = "";
      dbConf.host = "";
    }
  }
});

app.post("/logoutDB", async function (req, res) {
  if (dbConf.database == "" || dbConf.password == "" || dbConf.user == "" || dbConf.host == "") {
    res.status(400).send({ message: "Login First to Logout From Database." });
  } else {
    if (connection && connection.end) {
      connection.end();
      res.send({ message: "Logged Out Successfully." });
    } else {
      res.send({ message: "Connection abrupted. Login Again" });
    }

    dbConf.user = "";
    dbConf.password = "";
    dbConf.database = "";
    dbConf.host = "";
  }
});



app.post("/DBstore", async function (req, res) {
  if (dbConf.database == "" || dbConf.password == "" || dbConf.user == "") {
    res.status(400).send({ message: "Login First to Store in Database." });
  } else {
    /* Button will not be showing only so empty case cannot be there */
    fs.readdir(upload_path, (err, files) => {
      files.forEach((file) => {
        var in_there = false;

        async function insert_file(in_there) {
          const filename = path.join(__dirname, "/uploads/", file);
          let g1 = sharedLib.gpxfile_to_json(filename);
          let existsquery =
            "SELECT COUNT(file_name) AS count FROM FILE WHERE file_name = '" +
            path.basename(file) +
            "';";

          let already_exit;
          try {
            already_exit = await connection.execute(existsquery);
          } catch (e) {
            console.log("Query error: " + e);
            return res.status(400).send({
              message:
                "Failed to Store the Files. Use Clear Tables to Clean the Suspicious",
            });
          }

          let y = JSON.stringify(already_exit[0]);
          let z = JSON.parse(y);

          if (z[0].count == 0) {
            in_there = true;
            let j1 = JSON.parse(g1);
            let query1 =
              "INSERT INTO FILE  (file_name,ver,creator) VALUES ('" +
              path.basename(file) +
              "' ," +
              j1.version +
              ", '" +
              j1.creator +
              "');";

            try {
              await connection.execute(query1);
            } catch (e) {
              console.log("Query error: " + e);
              return res.status(400).send({
                message:
                  "Failed to Store the Files. Use Clear Tables to Clean the Suspicious",
              });
            }


            let query2 =
              "SELECT gpx_id FROM FILE where file_name = '" +
              path.basename(file) +
              "';";

            let x1;

            try {
              x1 = await connection.execute(query2)
            } catch (e) {
              console.log("Query error: " + e);
              return res.status(400).send({
                message:
                  "Failed to Store the Files. Use Clear Tables to Clean the Suspicious",
              });
            }

            let y1 = JSON.stringify(x1[0]);
            let z1 = JSON.parse(y1);
            let r1 = sharedLib.get_all_routes(filename);

            let rj1 = JSON.parse(r1);
            // console.log(rj1);

            // console.log("Reached");
            if (rj1.length > 0) {
              for (var i = 0; i < rj1.length; i++) {
                let query =
                  "INSERT INTO ROUTE  (route_name,route_len,gpx_id) VALUES ('" +
                  rj1[i].name +
                  "' ," +
                  rj1[i].len +
                  ", " +
                  z1[0].gpx_id +
                  ");";
                // console.log(query);

                try {
                  await connection.execute(query);
                } catch (e) {
                  console.log("Query error: " + e);
                  return res.status(400).send({
                    message:
                      "Failed to Store the Files. Use Clear Tables to Clean the Suspicious",
                  });
                }

                /* Waypoints Filling */
                let query3 =
                  "SELECT route_id FROM ROUTE where gpx_id = " +
                  z1[0].gpx_id +
                  "  AND route_name = '" +
                  rj1[i].name +
                  "' AND route_len = " +
                  rj1[i].len
                ";";

                let x2;
                try {
                  x2 = await connection.execute(query3);
                } catch (e) {
                  console.log("Query error: " + e);
                  return res.status(400).send({
                    message:
                      "Failed to Store the Files. Use Clear Tables to Clean the Suspicious",
                  });
                }

                let y2 = JSON.stringify(x2[0]);
                // console.log(y2);
                let z2 = JSON.parse(y2);
                let n = z2.length;
                // console.log(z2[n - 1].route_id);

                /* GET FROM GPX PARSER */

                let swp = sharedLib.listWP_Route(filename, i + 1);
                // console.log("I : " + i + " JSON ALL : " + swp);

                let wpALL = JSON.parse(swp);
                // console.log(wpALL);

                for (var j in wpALL) {
                  if (wpALL[j].name.length > 0) {
                    let query_points =
                      "INSERT INTO POINT  (point_index,latitude,longitude,point_name,route_id) VALUES (" +
                      wpALL[j].index +
                      " ," +
                      wpALL[j].lat +
                      ", " +
                      wpALL[j].lon +
                      ",'" +
                      wpALL[j].name +
                      "', " +
                      z2[n - 1].route_id +
                      ");";
                    // console.log(query_points);

                    try {
                      await connection.execute(query_points);
                    } catch (e) {
                      console.log("Query error: " + e);
                      return res.status(400).send({
                        message:
                          "Failed to Store the Files. Use Clear Tables to Clean the Suspicious",
                      });
                    }

                  }
                  else {
                    let query_points =
                      "INSERT INTO POINT  (point_index,latitude,longitude,route_id) VALUES (" +
                      wpALL[j].index +
                      " ," +
                      wpALL[j].lat +
                      ", " +
                      wpALL[j].lon +
                      ", " +
                      z2[n - 1].route_id +
                      ");";
                    // console.log(query_points);

                    try {
                      await connection.execute(query_points);
                    } catch (e) {
                      console.log("Query error: " + e);
                      return res.status(400).send({
                        message:
                          "Failed to Store the Files. Use Clear Tables to Clean the Suspicious",
                      });
                    }
                  }
                }
              }
            }
          }
        }
        insert_file(in_there);
      });
    });

    res.send({ message: 'Successfully Stored to Database.' });
  }
});



app.post("/DBclear", async function (req, res) {
  if (dbConf.database == "" || dbConf.password == "" || dbConf.user == "") {
    res.status(400).send({ message: "Login First to Access Database." });
  }
  else {
    let q1 = 'SELECT EXISTS (SELECT * FROM FILE);';
    let r1;

    try {
      let temp = await connection.execute(q1);
      let r = JSON.stringify(temp[0]);
      r1 = JSON.parse(r);
    } catch (e) {
      console.log("Query error: " + e);
      return res.status(400).send({
        message:
          "Failed to Clear the Files. Use Clear Tables to Clean the Suspicious",
      });
    }

    if (r1[0]["EXISTS (SELECT * FROM FILE)"] == 0) {
      return res.status(400).send({
        message:
          "Tables are Already Empty!!",
      });
    }
    else {
      let query = 'DELETE FROM FILE';
      try {
        let temp = await connection.execute(query);
        let r = JSON.stringify(temp[0]);
        r1 = JSON.parse(r);
      } catch (e) {
        console.log("Query error: " + e);
        return res.status(400).send({
          message:
            "Cannot Delete the Files! Please Try Again.",
        });
      }
    }

    let q11 = 'ALTER TABLE FILE AUTO_INCREMENT = 1;';
    let q12 = 'ALTER TABLE ROUTE AUTO_INCREMENT = 1;';
    let q13 = 'ALTER TABLE POINT AUTO_INCREMENT = 1;';

    try {
      await connection.execute(q11);
    } catch (e) {
      console.log("Query error: " + e);
      return res.status(400).send({
        message:
          "Cannot Alter the Auto Increament of Table.",
      });
    }
    try {
      await connection.execute(q12);
    } catch (e) {
      console.log("Query error: " + e);
      return res.status(400).send({
        message:
          "Cannot Alter the Auto Increament of Table.",
      });
    }
    try {
      await connection.execute(q13);
    } catch (e) {
      console.log("Query error: " + e);
      return res.status(400).send({
        message:
          "Cannot Alter the Auto Increament of Table.",
      });
    }
    res.send({ message: 'Tables Cleared Successfully.' });

  }
});


app.post("/DBshow", async function (req, res) {
  if (dbConf.database == "" || dbConf.password == "" || dbConf.user == "") {
    res.status(400).send({ message: "Login First to Access Database." });
  }
  else {
    let q1 = 'SELECT COUNT(*) AS num_files FROM FILE;';
    let q2 = 'SELECT COUNT(*) AS num_routes FROM ROUTE;';
    let q3 = 'SELECT COUNT(*) AS num_points FROM POINT;';
    let files;
    let routes;
    let points;

    try {
      let temp = await connection.execute(q1);
      let r = JSON.stringify(temp[0]);
      let x = JSON.parse(r);
      files = x[0].num_files
    } catch (e) {
      console.log("Query error: " + e);
      return res.status(400).send({
        message:
          "Failed to Clear the Files. Use Clear Tables to Clean the Suspicious",
      });
    }
    try {
      let temp = await connection.execute(q2);
      let r = JSON.stringify(temp[0]);
      let x = JSON.parse(r);
      routes = x[0].num_routes
    } catch (e) {
      console.log("Query error: " + e);
      return res.status(400).send({
        message:
          "Failed to Clear the Files. Use Clear Tables to Clean the Suspicious",
      });
    }
    try {
      let temp = await connection.execute(q3);
      let r = JSON.stringify(temp[0]);
      let x = JSON.parse(r);
      points = x[0].num_points;
    } catch (e) {
      console.log("Query error: " + e);
      return res.status(400).send({
        message:
          "Failed to Clear the Files. Use Clear Tables to Clean the Suspicious",
      });
    }

    let send_message = 'Database has ' + files + ' files, ' + routes + ' routes and ' + points + ' waypoints.';
    res.send({ message: send_message });

  }
});


/* EXECUTE QUERIES */

app.get("/db_rtfile_dd", function (req, res) {
  var names = new Array();
  fs.readdir(upload_path, (err, files) => {
    files.forEach((file) => {
      let nm = '{"name":"' + path.basename(file) + '"}';
      let s = JSON.parse(nm);
      names.push(s);
    });

    res.send(names);
  });
});

app.post("/allrts_name", async function (req, res) {

  let query = 'SELECT * FROM ROUTE ORDER BY route_name ASC;';
  try {
    const [rows1, fields1] = await connection.execute(query);
    if (rows1.length == 0) {
      return res.status(400).send({
        message:
          "ROUTE table is Empty!!",
      });
    }
    return res.json(rows1);

  } catch (e) {
    console.log("Query error: " + e);
    return res.status(400).send({
      message:
        "Cannot get the routes from the database. Please Try again",
    });
  }

  // console.log(result);

});


app.post("/allrts_len", async function (req, res) {

  let query = 'SELECT * FROM ROUTE ORDER BY route_len ASC;';
  try {
    const [rows1, fields1] = await connection.execute(query);
    if (rows1.length == 0) {
      return res.status(400).send({
        message:
          "ROUTE table is Empty!!",
      });
    }
    return res.json(rows1);

  } catch (e) {
    console.log("Query error: " + e);
    return res.status(400).send({
      message:
        "Cannot get the routes from the database. Please Try again",
    });
  }

  // console.log(result);

});

var show_all_route_file = "";

app.post("/rt_file", function (req, res) {
  var t_n = req.body.name;
  show_all_route_file = t_n;
});

app.post("/file_rts_name", async function (req, res) {

  if (show_all_route_file == "") {
    return res.status(400).send({
      message:
        "Choose a GPX File to Display Its Routes.",
    });
  }
  let GPXID;
  let query = "SELECT gpx_id FROM FILE WHERE file_name = '" + show_all_route_file + "';";
  try {
    let x = await connection.execute(query);
    let y = JSON.stringify(x[0]);
    let z = JSON.parse(y);
    GPXID = z[0].gpx_id;


  } catch (e) {
    console.log("Query error: " + e);
    return res.status(400).send({
      message:
        "Cannot get the routes from the database. Please Try again",
    });
  }
  let query2 = 'SELECT * FROM ROUTE WHERE gpx_id =' + GPXID + ' ORDER BY route_name ASC;';
  try {
    const [rows1, fields1] = await connection.execute(query2);
    if (rows1.length == 0) {
      return res.status(400).send({
        message:
          "ROUTE table is Empty for file " + show_all_route_file,
      });
    }
    return res.json(rows1);

  } catch (e) {
    console.log("Query error: " + e);
    return res.status(400).send({
      message:
        "Cannot get the routes from the database. Please Try again",
    });
  }
});

app.post("/file_rts_len", async function (req, res) {

  if (show_all_route_file == "") {
    return res.status(400).send({
      message:
        "Choose a GPX File to Display Its Routes.",
    });
  }
  let GPXID;
  let query = "SELECT gpx_id FROM FILE WHERE file_name = '" + show_all_route_file + "';";
  try {
    let x = await connection.execute(query);
    let y = JSON.stringify(x[0]);
    let z = JSON.parse(y);
    GPXID = z[0].gpx_id;


  } catch (e) {
    console.log("Query error: " + e);
    return res.status(400).send({
      message:
        "Cannot get the routes from the database. Please Try again",
    });
  }
  let query2 = 'SELECT * FROM ROUTE WHERE gpx_id =' + GPXID + ' ORDER BY route_len ASC;';
  try {
    const [rows1, fields1] = await connection.execute(query2);
    if (rows1.length == 0) {
      return res.status(400).send({
        message:
          "ROUTE table is Empty for file " + show_all_route_file,
      });
    }
    return res.json(rows1);

  } catch (e) {
    console.log("Query error: " + e);
    return res.status(400).send({
      message:
        "Cannot get the routes from the database. Please Try again",
    });
  }
});


    // console.log(result);


