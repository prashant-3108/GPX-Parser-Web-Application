
# GPX Parser Web Application
This is a Web Application that allow users to work with the GPX Files,get information from them and mosify the files, add routes, access remote databases using VPN,etc.

- Developed an algorithm to Validate and Parse a GPX File and converting into a GPX Document having List of Waypoints
together signifying Routes/Tracks with necessary information using Clang & XML Parser Modules and Libraries (libxml2).
- Developed a User Interface (Javascript,Ajax,Jquery) which interacts with client to display & modify the information about GPX Files.
- Developed a RESTful API (Expess.js) which interacts with the developed C backend with “ffi-napi” using JSON strings to process the client queries like downloading/uploading files to server, Adding Routes, Modifying Routes/Tracks,
etc. 
- UI supports interaction to the remote databases also allowing user to perform CRUD operations on GPX files and components into database.

<br>

![Globe](/sample_gpx_files/globe.jpg)

<br>


## Backend Installation


###### To run the application locally you need to have a linux distro. 
###### [ Will not be working locally in Windows and other operating systems ]


##### Ubuntu :
```Bash
# Update the current repositories
sudo apt update;sudo apt uprade;
```

```Bash
# Install libxml2
sudo apt-get install libxml2
sudo apt-get install libxml2-dev
```

###### If you are having different linux distro, just google and install libxml2 & libxml2-dev.

##### Now Get to the Parser Directory

```Bash
# Getting to the parser directory.
cd parser
```
##### Run Makefile


```Bash
# Run the makefile to get the shared library .so
make
```

## Installation of Node App

### 1. Install

```Bash
# From the root of the directory
npm install
```

### 2. Running Server

```Bash
# PORT eg. 8080,5500,etc
npm run dev PORT
# Server will be accessible at http://localhost:PORT
```

## Directory Structure

```Bash
# This contains the Backend Node Server, with our Web Application and API
app.js

# These are the package configuration files for npm to install dependencies
package.json
package-lock.json

# This is the Frontend HTML file that you see when you visit the document root
public/index.html

# This is the Frontend browser JavaScript file
public/index.js

# This is the Frontend Custom Style Sheet file
public/style.css

# This is the directory for uploaded .gpx files
upload/

# This is the directory where you put all your C parser code
parser/
```

<br>

## Functionalities 

#### 1. Upload and Download GPX Files at the Server.

User can upload GPX Files to the server and can download the existing Files in the server.
Only Valid GPX Files can be uploaded to the server **which passes the validation from the *"gpx.xsd"* following GPX schema version 1.1** which can be found in - 

    $ /parse/include/gpx.xsd


#### 2. GPX View. 

Get detailed information about the GPX Files in the server like:

    - Creator and Version of GPX files.
    - Length of Route/Track.
    - Number of Waypoints.
    - Whether Loop is formed by the Route/Track.

#### 3. Create New GPX File. 

Create a GPX Document which is saved to an GPX file where user enters name, creator and  version of the GPX File. After successful creation, this file will be available in the server as well as in List of GPX files in user interface. 

#### 3. Add Route. 

Select a GPX file from the files in the list and add route to it by specifying the Waypoints (single/multiple) and adding Route name to the inserted route. After successful addition, this file will be updated in the server as well as in List of GPX file informations of UI.

#### 3. Change Component Name.

Select a GPX file from the files in the list and a corresponding component route/track and update its name in the gpx file at server. Updated at UI too.

#### 4. Find All paths between two Waypoints.

This functionality will give the list of all the routes and tracks that have these waypoints in them with some tolerance (error value).


#### 5. Show Other Properties of the Routes/Tracks.

A route/track can have different other properties like elevation,description,date and time,etc. This functionality will show all those in a tabular form for the selected component by user.

#### 6. Remote Access to Databases

DATABASE TABLES : 

a. FILE - stores files info
b. ROUTE - stores all routes info
c. POINT - stores all the points of every route

##### -> Login/Logout to Database: 

The UI asks the user to enter the hostname/IP address, username, password, and database name, and will attempt to create a connection. If the connection fails, it displays an alert and prompt the user to re-enter the hostname/IP address,username, DB name, and password again.

*If you are not able to access a database remotely, you can connect to **VPN** and then try to login.*

##### ->  Store all files in server to remote Database:

##### -> Clear all table Data in server to remote Database.

- This Functionality **Clears** the Database Tables *rather than dropping the tables*.

##### -> Display DB Status.
This displays an alert with the status :

    "Database has N1 files, N2 routes, and N3 points".

##### -> Route Updates.

If user updates in the routes or add new routes if database is logged in it will ask if you want to add/modify route to remote database too. If Yes then it will update the files as well as Remote DB Tables ROUTE + POINT.


<br>


### HTTP Web Server
Used NodeJS runtime to create a simple web server.

### Web Application Framework (Web Application, API)


An Application Programming Interface (API) is essentially just an interface, we're using to serve our set of routes for the client browser JavaScript to interact using HTTP protocol to access Backend functionality.

Created a RESTful API: https://restfulapi.net/ using Express.js

HTTP Methods to consider:

* GET: read data (nothing changes)

* POST: create data

* PUT: update data

* DELETE: delete data

In Express it's very simple to create a single "route". A route is just an endpoint you can access from your JavaScript


### NodeJS Libraries

```JavaScript
// Strict Mode
'use strict'

// This gives us direct access to C functions from our library
const ffi = require('ffi-napi');

// Express App library
const express = require("express");
const app     = express();

// Path utility library
const path    = require("path");

// File Upload library
const fileUpload = require('express-fileupload');
app.use(fileUpload());

// File reading and manipulating library
const fs = require('fs');

// Minimization, this is to obfuscate our JavaScript
// Obfuscation and Minimization are ways to reduce payload size
// And to get scripts to clients quicker because of the smaller size
const JavaScriptObfuscator = require('javascript-obfuscator');
```

## How does everything work together?

1. Install the dependencies (only need to do this once) and spin up your node server as described in installation above.

*Note: We're using "nodemon" (instead of say `node run dev`) because it hot-reloads app.js whenever it's changed*

2. View Web Application at http://localhost:PORT

3. The HTML is loaded when you visit the page and see forms and contents.

4. The CSS is also loaded, and you'll see the page has style. 

5. The JavaScript file is loaded (index.js) and will run a bunch of "on load" AJAX calls to populate dropdowns, change elements.

6. When buttons are clicked, more AJAX calls are made to the backend, that recieve a response update the HTML.

7. An AJAX call is made from your browser, it makes an HTTP (GET, POST...) call to our web server.

8. The app.js web server receives the request with the route, and request data (JSON, url parameters, files...).

9. Express looks for the route you defined, then runs the callback function you provided.

10. The callback function (for this module) should just return a hard coded JSON response

11. The AJAX call gets a response back from our server (either a 200 OK or maybe an error like a 404 not found) and either calls the "success" callback function or the "fail" function.
