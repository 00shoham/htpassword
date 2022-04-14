var baseURL = "/cgi-bin/htpassword?api";

function DisplayError( msg )
  {
  var errDiv = document.getElementById("error");
  if( typeof errDiv != "undefined" )
    errDiv.innerHTML = msg;
  else
    alert( msg );
  }

function sampleonload()
  {
  var responseText = this.responseText; 
  var responseObj;
  if( typeof responseText != "undefined" )
    {
    try
      {
      responseObj = JSON.parse( responseText );
      }
    catch(err)
      {
      DisplayError( "Failed to parse JSON from string ["+responseText+"] - err="+err );
      }
    }

  var errmsg = "An error has occurred.  No details available.";

  if( typeof responseObj == "undefined"
      || typeof responseObj.code == "undefined"
      || typeof responseObj.result == "undefined" )
    {
    DisplayError( errmsg + " (responseObj or components undefined - responseText=[" + responseText + "]) ");
    }
  else if( displayErrors && responseObj.code !=0 )
    {
    errmsg = responseObj.result + " ("+responseObj.code+")";
    DisplayError( errmsg );
    }

  DisplayError( "All is well with the callback" );
  }

function sampleonerror()
  {
  errmsg = this.statusText;
  DisplayError( "Attempt to call an API function returned "+errmsg );
  }

function OnLoadStrengthCheck()
  {
  var responseText = this.responseText; 
  var responseObj;
  if( typeof responseText != "undefined" )
    {
    try
      {
      responseObj = JSON.parse( responseText );
      }
    catch(err)
      {
      DisplayError( "Failed to parse JSON from string ["+responseText+"] - err="+err );
      }
    }

  var errmsg = "An error has occurred.  No details available.";

  if( typeof responseObj == "undefined"
      || typeof responseObj.code == "undefined"
      || typeof responseObj.result == "undefined" )
    {
    DisplayError( errmsg + " (responseObj or components undefined - responseText=[" + responseText + "]) ");
    }
  else if( responseObj.code != 0 )
    {
    var html = "";
    if( typeof responseObj.CONSTRAINTS != "undefined" )
      {
      var gotAny = 0;
      for( var i=0; i<responseObj.CONSTRAINTS.length; i++ )
        {
        if( gotAny )
          html += "<br/>\n";
        html += responseObj.CONSTRAINTS[i];
        gotAny = 1;
        }
      DisplayPasswordError( html );
      }
    else
      {
      DisplayPasswordError( "Password not acceptable but the server gave no details." );
      }
    }
  }

function sampleonerror()
  {
  errmsg = this.statusText;
  DisplayError( "Attempt to call an API function returned "+errmsg );
  }

function CallAPIFunctionASync( topic, action, args, callbackonload, callbackonerror )
  {
  var method = "/" + topic + "/" + action;
  var xhttp = new XMLHttpRequest();
  xhttp.open( "POST", baseURL + method, true );
  xhttp.onload = callbackonload;
  xhttp.onerror = callbackonerror;
  xhttp.send( JSON.stringify(args) );
  }

function CallAPIFunctionSync( topic, action, args, displayErrors )
  {
  var method = "/" + topic + "/" + action;
  var xhttp = new XMLHttpRequest();
  xhttp.open( "POST", baseURL + method, false );
  xhttp.send( JSON.stringify(args) );
  var responseText = xhttp.responseText; 
  var responseObj;
  if( typeof responseText != "undefined" )
    {
    try
      {
      responseObj = JSON.parse( responseText );
      }
    catch(err)
      {
      DisplayError( "Failed to parse JSON from string ["+responseText+"] - err="+err );
      }
    }

  var errmsg = "An error has occurred.  No details available.";

  if( typeof responseObj == "undefined"
      || typeof responseObj.code == "undefined"
      || typeof responseObj.result == "undefined" )
    {
    DisplayError( errmsg + " (responseObj or components undefined - responseText=[" + responseText + "]) ");
    }
  else if( displayErrors && responseObj.code !=0 )
    {
    errmsg = responseObj.result + " ("+responseObj.code+")";
    DisplayError( errmsg );
    }

  return responseObj;
  }

function ToggleObjectDisplay( id )
  {
  var obj = document.getElementById( id );
  if( ! obj || typeof obj == "undefined" )
    return;
  if( obj.style.display == "none")
    obj.style.display = "block";
  else
    obj.style.display = "none";
  }

function ReportResults( msg )
  {
  var errDiv = document.getElementById("error");
  if( typeof errDiv != "undefined" )
    errDiv.innerHTML = msg;
  }

// main function to render the UI.
var tabIsVisible = false;
function VisibilityChanged()
  {
  if( document.visibilityState === 'visible')
    {
    tabIsVisible = true;
    Refresh();
    }
  else
    {
    tabIsVisible = false;
    }
  }

var passwordFileID = '';
var passwordFileDesc = '';
var numPasswordFiles = 0;

function ListPasswordFiles()
  {
  var args = {};
  var obj = CallAPIFunctionSync( "file", "list", args, true );
  if( typeof obj == "undefined"
      || typeof obj.FILES == "undefined")
    return [];
  var list = obj['FILES'];

  if( typeof list == "undefined"
      || typeof list.length == "undefined"
      || list.length<1 )
    return [];
  else
    {
    if( numPasswordFiles != list.length )
      {
      numPasswordFiles = list.length;
      }
    return list;
    }
  }

var counter = 0;

function SelectPasswordFile( id, desc )
  {
  passwordFileID = id;
  passwordFileDesc = desc;
  console.log( "Selected password file "+passwordFileID+" - "+passwordFileDesc );
  ++counter;
  if( counter > 10 )
    alert( "Stop!" );
  }

function SelectPasswordFileAndRefresh( id, desc )
  {
  SelectPasswordFileAndRefresh( id, desc );
  Refresh();
  }

function PasswordListChooser( list )
  {
  var html = "";
  html  = "<p>Select a password to change:</p>\n";
  html += "<ul>\n";
  for( var i=0; i<list.length; ++i )
    {
    var single = list[i];
    if( typeof single != "undefined"
        && typeof single.ID != "undefined" 
        && typeof single.DESCRIPTION != "undefined"  )
      {
      html += "  <li class='passwordFile-chooser'><a class='passwordFile-chooser' href='javascript:;' onclick='SelectPasswordFileAndRefresh(\"" + single.ID + "\",\"" + single.DESCRIPTION + "\")'>" + list[i].DESCRIPTION + "</a></li>\n";
      }
    }

  html += "</ul>\n";
  return html;
  }

function ChoosePasswordFile()
  {
  var htmlContainer = document.getElementById( "content" );
  if( typeof htmlContainer != "undefined" )
    {
    passwordFileID = '';
    passwordFileDesc = '';
    Refresh();
    }
  }

function DisplayPasswordError( msg )
  {
  var errDiv = document.getElementById("password-error");
  if( typeof errDiv != "undefined" )
    errDiv.innerHTML = msg;
  else
    alert( msg );
  }

function GetUserID()
  {
  var userID = document.getElementById( 'user-id' );
  if( typeof userID == "undefined" )
    {
    DisplayPasswordError( "Cannot locate user ID" );
    return "";
    }
  var userIDstr = userID.innerHTML;
  if( userIDstr == "" )
    {
    DisplayPasswordError( "User ID is empty" );
    return "";
    }

  return userIDstr;
  }

function GetOldP()
  {
  var oldPW = document.getElementById( 'old-password' );
  if( typeof oldPW == "undefined" )
    {
    DisplayPasswordError( "Cannot locate old password" );
    return "";
    }
  var oldPWstr = oldPW.value;
  if( oldPWstr == "" )
    {
    DisplayPasswordError( "Old password is empty" );
    return "";
    }

  return oldPWstr;
  }

function GetNewP()
  {
  var newPW = document.getElementById( 'new-password' );
  if( typeof newPW == "undefined" )
    {
    DisplayPasswordError( "Cannot locate new password" );
    return "";
    }
  var newPWstr = newPW.value;
  if( newPWstr == "" )
    {
    DisplayPasswordError( "New password is empty" );
    return "";
    }

  return newPWstr;
  }

function GetAgainP()
  {
  var againPW = document.getElementById( 'new-password-2' );
  if( typeof againPW == "undefined" )
    {
    DisplayPasswordError( "Cannot locate second new password" );
    return "";
    }
  var againPWstr = againPW.value;
  if( againPWstr == "" )
    {
    DisplayPasswordError( "Second new password is empty" );
    return "";
    }

  return againPWstr;
  }

function CheckPasswords( context, userID, oldPW, newPW, againPW )
  {
  if( userID=="" )
    return -1;

  if( context=="change" || context=="oldPW" )
    {
    if( oldPW=="" )
      return -2;
    }

  if( context=="change" || context=="newPW" )
    {
    if( newPW=="" )
      {
      return -3;
      }

    if( oldPW==newPW )
      {
      DisplayPasswordError( "Old and new passwords must differ" );
      return -4;
      }

    var args = { "ID":passwordFileID,
                 "USERID":userID,
                 "PASSWORD":newPW
               };
    CallAPIFunctionASync( "password", "checkstrength", args,
                          OnLoadStrengthCheck, sampleonerror );
    }

  if( context=="change" || context=="againPW" )
    {
    if( newPW != againPW )
      {
      DisplayPasswordError( "Passwords do not match" );
      return -7;
      }
    }

  return 0;
  }

function ChangePassword()
  {
  var retVal = false;

  var errDiv = document.getElementById("password-error");
  if( typeof errDiv != "undefined" )
    errDiv.innerHTML = "";

  var userID = GetUserID();
  if( userID == "" )
    return;

  var oldPW = GetOldP();
  if( oldPW == "" )
    return;

  var newPW = GetNewP();
  if( newPW == "" )
    return;

  var againPW = GetAgainP();
  if( againPW == "" )
    return;

  if( CheckPasswords( "change", userID, oldPW, newPW, againPW )!=0 )
    return;

  var args = { "ID":passwordFileID,
               "USERID":userID,
               "OLDP":oldPW,
               "NEWP":newPW
             };
  var obj = CallAPIFunctionSync( "password", "changepassword", args, false );
  if( typeof obj != "undefined"
      && typeof obj.code != "undefined" 
      && typeof obj.result != "undefined" 
      )
    {
    if( obj.code==0 )
      DisplayPasswordError( "Success!" );
    else
      DisplayPasswordError( obj.result );
    }
  }

function MyUserID()
  {
  var args = {};
  var obj = CallAPIFunctionSync( "user", "whoami", args, true );
  if( typeof obj != "undefined"
      && typeof obj.USERID != "undefined")
    {
    return obj.USERID;
    }
  return "";
  }

function PasswordChangeDialog( id, desc )
  {
  var html = "";

  if( numPasswordFiles > 1 )
    {
    html += "<p class='choose-password-file'><a class='choose-password-file' href='javascript:;' onclick='ChoosePasswordFile();'>Select a different password file</a></p>\n";
    }

  html += "<p class='password-dialog-title'>Change password in "+desc+"</p>\n";

  html += "    <div class='password-action'>\n";
  html += "      <table class='password-change-table'>\n";
  html += "        <tr class='password-change-row'>\n";
  html += "          <td class='password-change-label'>\n";
  html += "          </td>\n";
  html += "          <td class='password-change-value'>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='password-change-row'>\n";
  html += "          <td class='password-change-label'>\n";
  html += "            User ID:\n";
  html += "          </td>\n";
  html += "          <td class='password-change-value'>\n";
  html += "            <span id='user-id'>" + MyUserID() + "</span>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='password-change-row'>\n";
  html += "          <td class='password-change-label'>\n";
  html += "            Old password:\n";
  html += "          </td>\n";
  html += "          <td class='password-change-value'>\n";
  html += "            <input id='old-password' type='password' length=10 maxlength=50/>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='password-change-row'>\n";
  html += "          <td class='password-change-label'>\n";
  html += "            New password:\n";
  html += "          </td>\n";
  html += "          <td class='password-change-value'>\n";
  html += "            <input id='new-password' type='password' length=10 maxlength=50/>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='password-change-row'>\n";
  html += "          <td class='password-change-label'>\n";
  html += "            New password again:\n";
  html += "          </td>\n";
  html += "          <td class='password-change-value'>\n";
  html += "            <input id='new-password-2' type='password' ' length=10 maxlength=50/>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='password-change-row'>\n";
  html += "          <td class='password-change-label'>\n";
  html += "          </td>\n";
  html += "          <td class='password-change-value'>\n";
  html += "            <p>&nbsp;</p>\n";
  html += "            <a id='password-change-button' class='password-change-button' href='javascript:;' onclick='ChangePassword();'>Change passwords</a>\n";
  html += "            <p>&nbsp;</p>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "      </table>\n";
  html += "    </div>\n";

  html += "    <div class='password-error' id='password-error'></div>\n";

  return html;
  }

function MainPage()
  {
  var list = ListPasswordFiles();

  if( list.length==0 )
    return "<p>No password files defined.</p>\n";

  if( list.length==1 ) /* there is only one - do that */
    {
    var x = list[0];
    var y = x['ID'];
    var z = x['DESCRIPTION'];
    if( typeof y == "undefined" || typeof z == "undefined" )
      DisplayError( "Invalid password file ID or description" );
    else
      {
      SelectPasswordFile( y, z );
      return PasswordChangeDialog( y, z );
      }
    }

  if( typeof list != "undefined"
      && list.length > 1
      && passwordFileID == "" ) /* more than one but we don't know which */
    return PasswordListChooser( list );

  /* already selected the one previously */
  return PasswordChangeDialog( passwordFileID, passwordFileDesc );
  }

function DynamicPasswordValidation( event, context )
  {
  var userID = "";
  var oldPW = "";
  var newPW = "";
  var againPW = "";

  var errDiv = document.getElementById("password-error");
  if( typeof errDiv != "undefined" )
    errDiv.innerHTML = "";

  var userID = GetUserID();
  if( userID == "" )
    return;

  if( context=="oldPW" || context=="newPW" )
    {
    oldPW = GetOldP();
    if( oldPW == "" )
      return;
    }

  if( context=="newPW" || context=="againPW" )
    {
    newPW = GetNewP();
    if( newPW == "" )
      return;
    }

  if( context=="againPW" )
    {
    againPW = GetAgainP();
    if( againPW == "" )
      return;
    }

  CheckPasswords( context, userID, oldPW, newPW, againPW );
  }

function KeyPressOnAgain( event )
  {
  var x = event.keyCode;
  if( x == 13 ) // enter
    {
    ChangePassword();
    }
  }

function Refresh()
  {
  if( ! tabIsVisible )
    return;

  var htmlContainer = document.getElementById( "content" );
  if( typeof htmlContainer == "undefined" )
    {
    DisplayError( "Missing HTML container - content" );
    return;
    }

  var errorContainer = document.getElementById( "error" );
  if( typeof errorContainer == "undefined" )
    {
    DisplayError( "Missing HTML container - error" );
    return;
    }
  else
    errorContainer.innerHTML = "&nbsp;";

  htmlContainer.innerHTML = MainPage();

  var oldPW = document.getElementById( 'old-password' );
  if( typeof oldPW != "undefined" )
    {
    oldPW.addEventListener( 'input', function(event) { DynamicPasswordValidation(event, "oldPW"); }, false );
    oldPW.focus();
    }

  var newPW = document.getElementById( 'new-password' );
  if( typeof newPW != "undefined" )
    {
    newPW.addEventListener( 'input', function(event) { DynamicPasswordValidation(event, "newPW"); }, false );
    }

  var againPW = document.getElementById( 'new-password-2' );
  if( typeof againPW != "undefined" )
    {
    againPW.addEventListener( 'input', function(event) { DynamicPasswordValidation(event, "againPW"); }, false );
    againPW.addEventListener( 'keypress', KeyPressOnAgain, false );
    }

  }

function InitializeUI()
  {
  document.addEventListener( "visibilitychange", VisibilityChanged );
  tabIsVisible = true;
  Refresh();
  }

