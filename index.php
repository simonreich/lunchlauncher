<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
       "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
	<meta charset="utf-8" />
	<title>Lunch Launcher</title>
</head>

<body>

<?php
	// delete old messages and append new ones

	// set global variables
	$file = './launchlist.dat';
	$date = new DateTime ();
	$timestamp = $date->getTimestamp ();

	// messages older then $minutes will be deleted
	$minutes = 60;

	// delete old messages in file
	// open file
	$handle = @fopen ($file, "r");
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

	// set Name and text to be displayed
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

	// check if there are information in the url
	$newname = $_GET["name"];
	$newtext = $_GET["text"];
	if ($newname != "" && $newtext != "" )
	{
		$text = "${text}$timestamp;$newname;$newtext\n";
	};

	// check if there are information posted in the form
	$newname = $_POST["name"];
	$newtext = $_POST["text"];
	if ($newname != "" && $newtext != "" )
	{
		$text = "${text}$timestamp;$newname;$newtext\n";

		// set cookie
		setcookie("LunchLauncherName", $newname, time()+3600*24*30, "/lunchlauncher/", "fortknox.physik3.gwdg.de", FALSE, TRUE);
		setcookie("LunchLauncherText", $newtext, time()+3600*24*30, "/lunchlauncher/", "fortknox.physik3.gwdg.de", FALSE, TRUE);

		// update display text
		$setname = $newname;
		$settext = $newtext;
	};

	// write results to file
	file_put_contents ($file, $text, LOCK_EX);



	// Mail Alert

	// set global variables
	$filemail = './maillist.dat';

	// check if there are information in the url
	$newmail_GET = $_GET["mailaddress"];
	if ($newmail_GET != "")
	{
		$maillist = "${maillist}${newmail_GET}\n";
	};

	// check if there are information posted
	$newmail_POST = $_POST["mailaddress"];
	if ($newmail_POST != "")
	{
		$maillist = "${maillist}${newmail_POST}\n";
	};

	// check if we want to remove an email address
	$mailaddress_remove_POST = $_POST["mailaddress_remove"];
	$mailaddress_remove_GET = $_GET["mailaddress_remove"];

	// open file
	$handle = @fopen ($filemail, "r");
	if ($handle)
	{
		// read line by line
		while (($buffer = fgets ($handle, 4096)) !== false)
		{
			// check if mail address already exists
			if (${buffer} != "${newmail_POST}\n" && ${buffer} != "${newmail_GET}\n")
			{
				// check if mail address wants to be removed
				if (${buffer} != "${mailaddress_remove_POST}\n" && ${buffer} != "${mailaddress_remove_GET}\n")
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
	file_put_contents ($filemail, $maillist, LOCK_EX);



	// Send Mails

	// set global variables
	$fileOld = './launchlist_old.dat';

	// open file
	$handle = @fopen ($fileOld, "r");
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
				$textOld = "${textOld}$tags[0];$tags[1];$tags[2]";
			};
		};

		if (!feof($handle))
		{
			echo "Fehler: unerwarteter fgets() Fehlschlag\n";
		};
		fclose($handle);
	};

	$sendMail = "true";
	foreach(preg_split("/((\r?\n)|(\r\n?))/", $text) as $line)
	{
		foreach(preg_split("/((\r?\n)|(\r\n?))/", $textOld) as $lineOld)
		{
			// does the string contain ';'? Basi check for emptiness of weird stuff...
			if (strpos ($text, ';') !== false)
			{
				if ($text == $textOld)
				{
					$sendMail = "false";
				};
			} else {
				$sendMail = "false";
			};
		};
	};

	// send the mail to the list
	if ($sendMail == "true")
	{
		foreach(preg_split("/((\r?\n)|(\r\n?))/", $maillist) as $maillistLine)
		{
			// the foreach gives an almost empty list... So make a check for '@'
			if (strpos ($maillistLine, '@') !== false)
			{
				mail ($maillistLine, "Lunch has been launched!", "Dear colleagues,\n\nthe Lunch Launcher has been launched!\n\n${newname} said: \"${newtext}\".\n\nBon appetit,\n   The Lunch Launcher");
			}
		};
	};

	// All alarms should have been sent by now
	// write results to file
	file_put_contents ($fileOld, $text, LOCK_EX);

?>

<h1>Lunch Launcher</h1>

<h2>Add Launch</h2>
<form action="index.php" method="post">
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

<h2>Register for Mail-Alert</h2>
<p>Your mail address will be stored unencrypted and world readable!</p>
<form action="index.php" method="post">
	<p>Add Mail: <input type="text" name="mailaddress" value=""/></p>
	<p>Remove Mail: <input type="text" name="mailaddress_remove" value=""/></p>
	<p><input type="submit" value="Submit" /></p>

<h2>Mail-Alert List</h2>
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

</form>

</body>
</html>
