var baseURL = "/cgi-bin/htpassword?api";
var whichDialog = "change-password";

function DisplayError( msg )
  {
  var errDiv = document.getElementById("error");
  if( errDiv != null )
    errDiv.innerHTML = msg;
  else
    alert( msg );
  }

function sampleonload()
  {
  var responseText = this.responseText; 
  var responseObj;
  if( responseText != null )
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

  if( responseObj == null
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
  if( responseText != null )
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

  if( responseObj == null
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
      var errmsg = "Password not acceptable.";
      if( responseObj.result != null )
        errmsg += "  " + responseObj.result;
      DisplayPasswordError( errmsg );
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
  if( responseText != null )
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

  if( responseObj == null
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
  if( obj == null )
    return;
  if( obj.style.display == "none")
    obj.style.display = "block";
  else
    obj.style.display = "none";
  }

function ReportResults( msg )
  {
  var errDiv = document.getElementById("error");
  if( errDiv != null )
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
  if( obj == null
      || typeof obj.FILES == "undefined")
    return [];
  var list = obj['FILES'];

  if( list == null
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

function SelectPasswordFile( id, desc )
  {
  passwordFileID = id;
  passwordFileDesc = desc;
  console.log( "Selected password file "+passwordFileID+" - "+passwordFileDesc );
  }

function SelectPasswordFileAndRefresh( id, desc )
  {
  console.log( "SelectPasswordFileAndRefresh "+id+" - "+desc );
  if( id != null && id != "" )
    {
    SelectPasswordFile( id, desc );
    Refresh();
    }
  else
    {
    Refresh();
    }
  }

function PasswordListChooser( list )
  {
  var html = "";
  html  = "<p>Select a password to change:</p>\n";
  html += "<ul>\n";
  for( var i=0; i<list.length; ++i )
    {
    var single = list[i];
    if( single != null
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
  if( htmlContainer != null )
    {
    passwordFileID = '';
    passwordFileDesc = '';
    Refresh();
    }
  }

function DisplayPasswordError( msg )
  {
  var errDiv = document.getElementById("password-error");
  if( errDiv != null )
    errDiv.innerHTML = msg;
  else
    alert( msg );
  }

function GetUserID()
  {
  var userID = document.getElementById( 'user-id' );
  if( userID == null )
    {
    userID = document.getElementById( 'selected-user-id' );
    }
  if( userID == null )
    {
    DisplayPasswordError( "Cannot locate user ID" );
    return "";
    }
  var userIDstr = userID.innerHTML;
  if( userIDstr == "" )
    {
    userIDstr = userID.value;
    if( userIDstr == "" )
      {
      DisplayPasswordError( "User ID is empty" );
      return "";
      }
    }

  return userIDstr;
  }

function GetOldP()
  {
  // only makes sense if a user changes his own password..
  if( whichDialog != "change-password" )
    return "";

  var oldPW = document.getElementById( 'old-password' );
  if( oldPW == null )
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
  if( newPW == null )
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
  if( againPW == null )
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

  if( whichDialog == "change-password" )
    {
    if( context=="change" || context=="oldPW" )
      {
      if( oldPW=="" )
        return -2;
      }
    }

  if( context=="change" || context=="add" || context=="reset" || context=="newPW" )
    {
    if( newPW=="" )
      {
      return -3;
      }

    if( whichDialog == "change-password" && oldPW==newPW )
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

  if( context=="change" || context=="reset" || context=="add" || context=="againPW" )
    {
    if( newPW != againPW )
      {
      DisplayPasswordError( "Passwords do not match" );
      return -7;
      }

    var args = { "ID":passwordFileID,
                 "USERID":userID,
                 "PASSWORD":againPW
               };
    CallAPIFunctionASync( "password", "checkstrength", args,
                          OnLoadStrengthCheck, sampleonerror );
    }

  return 0;
  }

function ChangePassword()
  {
  var retVal = false;

  var errDiv = document.getElementById("password-error");
  if( errDiv != null )
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
  if( obj != null
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
  if( obj != null
      && typeof obj.USERID != "undefined")
    {
    return obj.USERID;
    }
  return "";
  }

function IsUserAdmin()
  {
  var args = { "ID":passwordFileID };
  var obj = CallAPIFunctionSync( "user", "isadmin", args, true );
  if( obj != null
      && typeof obj.ISADMIN != "undefined")
    {
    if( obj.ISADMIN == "true" )
      return true;
    else
      return false;
    }
  return false;
  }

function AdminOptions( id, desc )
  {
  var html = "";

  html += "<p class='admin-dialog-title'>\n";
  if( whichDialog != "change-password" )
    {
    html += "  &lt;<a class='admin-action' href='javascript:;' onclick='GoToChangePassword()'>Change my password</a>&gt;";
    html += "&nbsp;&nbsp;";
    }

  if( whichDialog != "add-user" )
    {
    html += "  &lt;<a class='admin-action' href='javascript:;' onclick='GoToAddUser()'>Add a user</a>&gt;";
    html += "&nbsp;&nbsp;";
    }

  if( whichDialog != "del-user" )
    {
    html += "  &lt;<a class='admin-action' href='javascript:;' onclick='GoToDelUser()'>Remove a user</a>&gt;";
    html += "&nbsp;&nbsp;";
    }

  if( whichDialog != "reset-password" )
    {
    html += "  &lt;<a class='admin-action' href='javascript:;' onclick='GoToResetPassword()'>Reset a password</a>&gt;";
    html += "&nbsp;&nbsp;";
    }
  html += "</p>\n";

  return html;
  }


function PasswordChangeDialog( id, desc )
  {
  whichDialog = "change-password";
  var html = "";

  if( numPasswordFiles > 1 )
    {
    html += "<p class='choose-password-file'><a class='choose-password-file' href='javascript:;' onclick='ChoosePasswordFile();'>Select a different password file</a></p>\n";
    }

  html += "<p class='password-dialog-title'>Change password in "+desc+"</p>\n";

  html += "    <div class='password-action'>\n";
  html += "      <table class='change-password-table'>\n";
  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "            User ID:\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "            <span id='user-id'>" + MyUserID() + "</span>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "            Old password:\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "            <input id='old-password' type='password' length=10 maxlength=50/>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "            New password:\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "            <input id='new-password' type='password' length=10 maxlength=50/>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "            New password again:\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "            <input id='new-password-2' type='password' ' length=10 maxlength=50/>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "            <p>&nbsp;</p>\n";
  html += "            <a id='change-password-button' class='change-password-button' href='javascript:;' onclick='ChangePassword();'>Change passwords</a>\n";
  html += "            <p>&nbsp;</p>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "      </table>\n";
  html += "    </div>\n";

  html += "    <div class='password-error' id='password-error'></div>\n";

  return html;
  }

function AddUser()
  {
  var retVal = false;

  console.log( "AddUser()" );

  var errDiv = document.getElementById("password-error");
  if( errDiv != null )
    errDiv.innerHTML = "";

  var userID = GetUserID();
  if( userID == "" )
    return;

  console.log( "AddUser() - userID = " + userID );

  var newPW = GetNewP();
  if( newPW == "" )
    return;

  console.log( "AddUser() - newPW = " + newPW );

  var againPW = GetAgainP();
  if( againPW == "" )
    return;

  console.log( "AddUser() - againPW = " + againPW );

  if( CheckPasswords( "add", userID, "", newPW, againPW )!=0 )
    return;

  var args = { "ID":passwordFileID,
               "USERID":userID,
               "PASSWORD":newPW
             };
  var obj = CallAPIFunctionSync( "user", "add", args, false );
  if( obj != null
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

function AddUserDialog( id, desc )
  {
  whichDialog = "add-user";
  var html = "";

  if( numPasswordFiles > 1 )
    {
    html += "<p class='choose-password-file'><a class='choose-password-file' href='javascript:;' onclick='ChoosePasswordFile();'>Select a different password file</a></p>\n";
    }

  html += "<p class='password-dialog-title'>Add a user to "+desc+"</p>\n";

  html += "    <div class='password-action'>\n";
  html += "      <table class='change-password-table'>\n";
  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "            User ID:\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "            <input id='user-id' type='text' length=10 maxlength=50/>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "            New password:\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "            <input id='new-password' type='password' length=10 maxlength=50/>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "            New password again:\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "            <input id='new-password-2' type='password' ' length=10 maxlength=50/>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "            <p>&nbsp;</p>\n";
  html += "            <a id='add-user-button' class='change-password-button' href='javascript:;' onclick='AddUser();'>Add user</a>\n";
  html += "            <p>&nbsp;</p>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "      </table>\n";
  html += "    </div>\n";

  html += "    <div class='password-error' id='password-error'></div>\n";

  html += AdminOptions();

  return html;
  }

function SelectUserID( id, desc )
  {
  var userIDfield = document.getElementById("selected-user-id");
  if( userIDfield != null )
    {
    userIDfield.innerHTML = id;

    var obj = document.getElementById("del-user-cotainer");
    if( obj != null )
      {
      obj.innerHTML = "<a id='del-user-button' class='change-password-button' href='javascript:;' onclick='DelUser();'>Remove selected user</a>";
      }

    }

  var userDescriptionfield = document.getElementById("selected-user-description");
  if( userDescriptionfield != null )
    {
    userDescriptionfield.innerHTML = desc
    }
  }

function ExistingUserListChooser( myUserID, list )
  {
  var html = "";

  if( myUserID == null || myUserID == "" )
    return "<p>Invalid logged in user ID</p>";

  html += "<ul>\n";
  for( var i=0; i<list.length; ++i )
    {
    var single = list[i];
    if( single != null
        && typeof single.ID != "undefined" )
      {
      if( single.ID != myUserID )
        {
        html += "  <li class='userID-chooser'><a class='userID-chooser' href='javascript:;' onclick='SelectUserID(\"" + single.ID + "\",\"\")'>" + list[i].ID + "</a></li>\n";
        }
      }
    }

  html += "</ul>\n";
  return html;
  }

function ListUsers( fileID )
  {
  console.log( "ListUsers( "+fileID+")" );

  var args = { "ID":fileID };
  var obj = CallAPIFunctionSync( "user", "list", args, true );
  if( obj == null
      || typeof obj.USERS == "undefined")
    {
    console.log( "ListUsers() --> empty" );
    return [];
    }
  var list = obj['USERS'];

  if( list == null
      || typeof list.length == "undefined"
      || list.length<1 )
    return [];
  else
    {
    return list;
    }
  }

function DelUser()
  {
  var retVal = false;
  console.log( "DelUser()" );

  var userID = document.getElementById("selected-user-id");
  if( userID == null )
    {
    DisplayError( "No user ID found" );
    return retVal;
    }

  userID = userID.innerHTML;
  if( userID == null
      || userID == ""
      || userID == MyUserID() )
    {
    DisplayError( "No user ID selected" );
    return retVal;
    }
  console.log( "DelUser() - userID = " + userID );

  var args = { "ID":passwordFileID,
               "USERID":userID
             };
  var obj = CallAPIFunctionSync( "user", "delete", args, false );
  if( obj != null
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

function DelUserDialog( id, desc )
  {
  console.log( "DelUserDialog()" );
  whichDialog = "del-user";
  var html = "";

  if( numPasswordFiles > 1 )
    {
    html += "<p class='choose-password-file'><a class='choose-password-file' href='javascript:;' onclick='ChoosePasswordFile();'>Select a different password file</a></p>\n";
    }

  html += "<p class='password-dialog-title'>Remove a user from "+desc+"</p>\n";

  html += "    <div class='password-action'>\n";
  html += "      <table class='change-password-table'>\n";
  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "            Selected user:\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  // html += "            ID: <span id='selected-user-id'></span>\n";
  // html += "            Description: <span id='selected-user-description'></span>\n";
  html += "            <span class='user-select-value' id='selected-user-id'></span>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "            Please select:\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  var list = ListUsers( id );
  var me = MyUserID();
  html += ExistingUserListChooser( me, list );
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "          </td>\n";
  html += "          <td id='del-user-cotainer' class='change-password-value'>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "      </table>\n";
  html += "    </div>\n";

  html += "    <div class='password-error' id='password-error'></div>\n";

  html += AdminOptions();

  console.log( "DelUserDialog() --> html == " + html );

  return html;
  }

function ResetPassword()
  {
  var retVal = false;

  console.log( "ResetPassword()" );

  var errDiv = document.getElementById("password-error");
  if( errDiv != null )
    errDiv.innerHTML = "";

  var userID = document.getElementById("selected-user-id");
  if( userID == null )
    {
    DisplayError( "No user ID found" );
    return retVal;
    }

  userID = userID.innerHTML;
  if( userID == null
      || userID == ""
      || userID == MyUserID() )
    {
    DisplayError( "No user ID selected" );
    return retVal;
    }
  console.log( "ResetPassword() - userID = " + userID );

  var newPW = GetNewP();
  if( newPW == "" )
    return;

  console.log( "ResetPassword() - newPW = " + newPW );

  var againPW = GetAgainP();
  if( againPW == "" )
    return;

  console.log( "ResetPassword() - againPW = " + againPW );

  if( CheckPasswords( "reset", userID, "", newPW, againPW )!=0 )
    return;

  var args = { "ID":passwordFileID,
               "USERID":userID,
               "PASSWORD":newPW
             };
  var obj = CallAPIFunctionSync( "password", "resetpassword", args, false );
  if( obj != null
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

function ResetPasswordDialog( id, desc )
  {
  whichDialog = "reset-password";
  var html = "";

  if( numPasswordFiles > 1 )
    {
    html += "<p class='choose-password-file'><a class='choose-password-file' href='javascript:;' onclick='ChoosePasswordFile();'>Select a different password file</a></p>\n";
    }

  html += "<p class='password-dialog-title'>Reset a password on "+desc+"</p>\n";

  html += "    <div class='password-action'>\n";
  html += "      <table class='change-password-table'>\n";
  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "            Selected user:\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  // html += "            ID: <span id='selected-user-id'></span>\n";
  // html += "            Description: <span id='selected-user-description'></span>\n";
  html += "            <span class='user-select-value' id='selected-user-id'></span>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "            Please select:\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  var list = ListUsers( id );
  var me = MyUserID();
  html += ExistingUserListChooser( me, list );
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "            New password:\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "            <input id='new-password' type='password' length=10 maxlength=50/>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "            New password again:\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "            <input id='new-password-2' type='password' ' length=10 maxlength=50/>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "        <tr class='change-password-row'>\n";
  html += "          <td class='change-password-label'>\n";
  html += "          </td>\n";
  html += "          <td class='change-password-value'>\n";
  html += "            <p>&nbsp;</p>\n";
  html += "            <a id='reset-password-button' class='change-password-button' href='javascript:;' onclick='ResetPassword();'>Reset Password</a>\n";
  html += "            <p>&nbsp;</p>\n";
  html += "          </td>\n";
  html += "        </tr>\n";

  html += "      </table>\n";
  html += "    </div>\n";

  html += "    <div class='password-error' id='password-error'></div>\n";

  html += AdminOptions();

  return html;
  }

function GoToAddUser()
  {
  whichDialog = "add-user";
  Refresh();
  }

function GoToDelUser()
  {
  whichDialog = "del-user";
  Refresh();
  }

function GoToResetPassword()
  {
  whichDialog = "reset-password";
  Refresh();
  }

function GoToChangePassword()
  {
  whichDialog = "change-password";
  Refresh();
  }

function MainPage()
  {
  console.log( "MainPage()" );

  var list = ListPasswordFiles();

  if( list.length==0 )
    return "<p>No password files defined.</p>\n";

  console.log( "MainPage() - list.length == " +list.length);

  if( list.length==1 ) /* there is only one - do that */
    {
    var x = list[0];
    passwordFileID = x['ID'];
    passwordFileDesc = x['DESCRIPTION'];
    if( passwordFileID == null || passwordFileDesc  == null )
      DisplayError( "Invalid password file ID or description" );
    else
      {
      if( whichDialog == "change-password" )
        {
        var isAdmin = IsUserAdmin();
        return PasswordChangeDialog( passwordFileID, passwordFileDesc )
               + (isAdmin ? AdminOptions( passwordFileID, passwordFileDesc ) : "");
        }
      else if( whichDialog == "add-user" )
        {
        return AddUserDialog( passwordFileID, passwordFileDesc );
        }
      else if( whichDialog == "del-user" )
        {
        return DelUserDialog( passwordFileID, passwordFileDesc );
        }
      else if( whichDialog == "reset-password" )
        {
        return ResetPasswordDialog( passwordFileID, passwordFileDesc );
        }
      }
    }

  if( list != null
      && list.length > 1
      && passwordFileID == "" ) /* more than one but we don't know which */
    return PasswordListChooser( list );

  /* already selected the one previously */
  console.log( "MainPage() - whichDialog == "+whichDialog );
  console.log( "MainPage() - passwordFileID == "+passwordFileID );
  console.log( "MainPage() - passwordFileDesc == "+passwordFileDesc );
  if( whichDialog == "change-password" )
    {
    var isAdmin = IsUserAdmin();
    return PasswordChangeDialog( passwordFileID, passwordFileDesc )
           + (isAdmin ? AdminOptions( passwordFileID, passwordFileDesc ) : "");
    }
  else if( whichDialog == "add-user" )
    {
    return AddUserDialog( passwordFileID, passwordFileDesc );
    }
  else if( whichDialog == "del-user" )
    {
    return DelUserDialog( passwordFileID, passwordFileDesc );
    }
  else if( whichDialog == "reset-password" )
    {
    return ResetPasswordDialog( passwordFileID, passwordFileDesc );
    }

  return "<p>whichDialog has an undefined value!</p>";
  }

function DynamicPasswordValidation( event, context )
  {
  var userID = "";
  var oldPW = "";
  var newPW = "";
  var againPW = "";

  var errDiv = document.getElementById("password-error");
  if( errDiv != null )
    errDiv.innerHTML = "";

  var userID = GetUserID();
  if( userID == "" )
    return;

  if( whichDialog == "change-password" )
    {
    if( context=="oldPW" || context=="newPW" )
      {
      oldPW = GetOldP();
      if( oldPW == "" )
        return;
      }
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
  console.log( "Refresh - tabIsVisible = "+tabIsVisible );
  if( ! tabIsVisible )
    return;

  var htmlContainer = document.getElementById( "content" );
  if( htmlContainer == null )
    {
    DisplayError( "Missing HTML container - content" );
    return;
    }

  var errorContainer = document.getElementById( "error" );
  if( errorContainer == null )
    {
    DisplayError( "Missing HTML container - error" );
    return;
    }
  else
    errorContainer.innerHTML = "&nbsp;";

  htmlContainer.innerHTML = MainPage();

  var oldPW = document.getElementById( 'old-password' );
  if( oldPW != null )
    {
    oldPW.addEventListener( 'input', function(event) { DynamicPasswordValidation(event, "oldPW"); }, false );
    oldPW.focus();
    }

  var newPW = document.getElementById( 'new-password' );
  if( newPW != null )
    {
    newPW.addEventListener( 'input', function(event) { DynamicPasswordValidation(event, "newPW"); }, false );
    }

  var againPW = document.getElementById( 'new-password-2' );
  if( againPW != null )
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

