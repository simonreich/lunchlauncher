<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
       "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
	<meta charset="utf-8" />
	<title>Lunch Launcher</title>
</head>

<body>

<?php
	// This file is part of Lunch Launcher
	//
	// Lunch Launcher is free software: you can redistribute it and/or modify
	// it under the terms of the GNU General Public License as published by
	// the Free Software Foundation, either version 3 of the License, or
	// (at your option) any later version.
	//
	// Lunch Launcher is distributed in the hope that it will be useful,
	// but WITHOUT ANY WARRANTY; without even the implied warranty of
	// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	// GNU General Public License for more details.
	//
	// You should have received a copy of the GNU General Public License
	// along with Lunch Launcher.  If not, see <http://www.gnu.org/licenses/>.



	// set global variables

	// The launch list is saved here. Clients must be able to download this file.
	$fileLunchlist = './launchlist.dat';

	// The mail list is saved here. Clients must not be able to download this file. Check the .htaccess file!
	$fileMail = './maillist.dat';

	// Address of the server. It is used in the mail notification.
	$serverAddress = 'https://fortknox.physik3.gwdg.de/lunchlauncher';

	// Name of this script. It is used in the mail notification and POST request / submit button.
	$serverFilename = 'index.php';

	// Messages older then $minutes will be delted upon calling this script.
	$minutes = 60;

	// Time settings. Please be aware that there is no time managemengt regarding server and client. NTP use is encouraged.
	$date = new DateTime ();
	$timestamp = $date->getTimestamp ();



	// No configuration after this.



	// set Name and text to be displayed in form: load from Cookie or set do default value
	if ($_COOKIE["LunchLauncherName"] != "")
	{
		$setname = $_COOKIE["LunchLauncherName"];
	} else {
		$setname = gethostbyaddr($_SERVER['REMOTE_ADDR']);
	};
	if ($_COOKIE["LunchLauncherText"] != "")
	{
		$settext = $_COOKIE["LunchLauncherText"];
	} else {
		$settext = "Lunch";
	};



	// Get new messages from either POST and GET and save into array

	// set mail notification to false and set it to true if new messages are here...
	$mailNotification =false;

	// check if there are information in the url = GET
	$newname = $_GET["name"];
	$newtext = $_GET["text"];
	if ($newname != "" && $newtext != "" )
	{
		$newtextArray[$newname] = $newtext;

		// send mail later
		$mailNotification = true;
	};

	// check if there are information posted in the form
	$newname = $_POST["name"];
	$newtext = $_POST["text"];
	if ($newname != "" && $newtext != "" )
	{
		$newtextArray[$newname] = $newtext;

		// User has visited the page - set cookie, valid for 30 days
		setcookie("LunchLauncherName", $newname, time()+3600*24*30, "/lunchlauncher/", "fortknox.physik3.gwdg.de", FALSE, TRUE);
		setcookie("LunchLauncherText", $newtext, time()+3600*24*30, "/lunchlauncher/", "fortknox.physik3.gwdg.de", FALSE, TRUE);

		// update display text
		$setname = $newname;
		$settext = $newtext;

		// send mail later
		$mailNotification = true;
	};



	// read old messages, delete old ones, and append new ones

	// delete old messages in file
	// open file
	$handle = @fopen ($fileLunchlist, "r");
	if ($handle)
	{
		// read line by line
		while (($buffer = fgets ($handle, 4096)) !== false)
		{
			// part line into array
			$tags = explode(';', $buffer);

			// check for time
			if ((int)(($timestamp-$tags[0])/60) <= (int)$minutes)
			{
				$text = "${text}$tags[0];$tags[1];$tags[2]";
			};
		};

		if (!feof($handle))
		{
			echo "Fehler: unerwarteter fgets() Fehlschlag\n";
		};
		fclose($handle);
	};

	// Append new messages
	foreach ($newtextArray as $newname => $newtext)
	{
		if ($newname !== "" && $newtext !== "")
		{
			$text = "${text}$timestamp;$newname;$newtext\n";
		};
	};

	// write results to file
	file_put_contents ($fileLunchlist, $text, LOCK_EX);



	// Mail Notification

	// check if we want to remove an email address
	$mailRemove[] = $_POST["mailremove"];
	$mailRemove[] = $_GET["mailremove"];

	// check if we want to add an email
	$mailAdd[] = $_POST["mailadd"];
	$mailAdd[] = $_GET["mailadd"];

	foreach ($mailAdd as $newmail)
	{
		// sanity check
		if (strpos ($newmail, '@') !== false)
		{
			$maillist = "${maillist}$newmail\n";
		};
	};

	// open file and read all mail addresses
	$handle = @fopen ($fileMail, "r");
	if ($handle)
	{
		// read line by line
		while (($buffer = fgets ($handle, 4096)) !== false)
		{
			$bufferSearch = str_replace (array("\n","\r"), '', $buffer);

			// check if mail address already exists
			if (!in_array (${bufferSearch}, $mailAdd))
			{
				// check if mail should be removed from list
				if (!in_array (${bufferSearch}, $mailRemove))
				{
					$maillist = "${maillist}$buffer";
				};
			};
		};

		if (!feof($handle))
		{
			echo "Fehler: unerwarteter fgets() Fehlschlag\n";
		};
		fclose($handle);
	};

	// write results to file
	file_put_contents ($fileMail, $maillist, LOCK_EX);

	// send mails
	if ($mailNotification == true)
	{
		// generate message text
		$mailtext = "Dear colleagues,\n\nthe Lunch Launcher has been launched!\n";
		foreach ($newtextArray as $newname => $newtext)
		{
			$mailtext = "${mailtext}${newname} said: \"${newtext}\"\n";
		};
		$mailtext = "${mailtext}\nBon appetit,\n   The Lunch Launcher";

		// send the mail to the list
		foreach(preg_split("/((\r?\n)|(\r\n?))/", $maillist) as $maillistLine)
		{
			// the foreach gives an almost empty list... So make a check for '@'
			if (strpos ($maillistLine, '@') !== false)
			{
				$mailtextPS = "\n\n\nPS: You can launch lunch at ${serverAddress} or unsubscribe by\nvisiting ${serverAddress}/${serverFilename}?mailremove=$maillistLine";
				mail ($maillistLine, "Lunch has been launched!", "${mailtext}${mailtextPS}");
			};
		};
	};
?>

<h1>Lunch Launcher</h1>

<h2>Add Launch</h2>
<form action="<?php echo $serverFilename; ?>" method="post">
	<p>Name: <input type="text" name="name" value="<?php echo $setname; ?>"/></p>
 	<p>Launch: <input type="text" name="text" value="<?php echo $settext; ?>"/></p>
	<p><input type="submit" value="Submit" /></p>
</form>

<h2>Full Launch List - last <?php echo $minutes?> minutes</h2>
<?php
	if ($text == "")
	{
		printf ("List is empty\n");
	} else {
		$textprint = nl2br($text);
		$textprint = str_replace(array("\n","\r"), '', $textprint);
		echo $textprint;
	};
?> 

<h2>Register for Mail Notification</h2>
<p>Your mail address will be stored unencrypted and world readable!</p>
<form action="<?php echo $serverFilename; ?>" method="post">
	<p>Add Mail: <input type="text" name="mailadd" value=""/></p>
	<p>Remove Mail: <input type="text" name="mailremove" value=""/></p>
	<p><input type="submit" value="Submit" /></p>
</form>

<h2>Mail Notification List</h2>
<?php
	if ($maillist == "")
	{
		printf ("List is empty\n");
	} else {
		$maillistprint = nl2br($maillist);
		$maillistprint = str_replace(array("\n","\r"), '', $maillistprint);
		$maillistprint = str_replace ("@" , " [at] " , $maillistprint);
		$maillistprint = str_replace ('.' , ' [dot] ' , $maillistprint);
		echo $maillistprint;
	};
?> 

<h2>Download latest script</h2>
<p>Please use the public available <a href="https://fortknox.physik3.gwdg.de/gitweb/?p=lunchlauncher.git;a=summary">git Repository</a>.</p>

</body>
</html>
