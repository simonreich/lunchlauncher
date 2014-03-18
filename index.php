<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
       "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
	<meta charset="utf-8" />
	<title>Lunch Launcher</title>
</head>

<body>

<?php
	// set global variables
	$fileLunchlist = './launchlist.dat';
	$fileMail = './maillist.dat';

	$date = new DateTime ();
	$timestamp = $date->getTimestamp ();

	// messages older then $minutes will be deleted
	$minutes = 60;



	// set Name and text to be displayed, load from Cookie
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
		$newtextArray[$newname] = $newtext

		// send mail later
		$mailNotification = true;
	};

	// check if there are information posted in the form
	$newname = $_POST["name"];
	$newtext = $_POST["text"];
	if ($newname != "" && $newtext != "" )
	{
		$newtextArray[$newname] = $newtext

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

	// Append new messages
	foreach ($newtextArray as $newname => $newtext)
	{
		$text = "${text}$timestamp;$newname$;$newtext";
	};

	// write results to file
	file_put_contents ($file, $text, LOCK_EX);



	// Mail Notification

	// check if we want to remove an email address
	$mailRemove_POST = $_POST["mailremove"];
	$mailRemove_GET = $_GET["mailremove"];

	// check if we want to add an email
	$mailAdd_POST = $_POST["mailadd"];
	$mailAdd_GET = $_GET["mailadd"];

	$maillist = "${maillist}${mailAdd_POST}${mailAdd_GET}";

	// open file and read all mail addresses
	$handle = @fopen ($fileMail, "r");
	if ($handle)
	{
		// read line by line
		while (($buffer = fgets ($handle, 4096)) !== false)
		{
			// check if mail address already exists
			if (${buffer} != "${mailAdd_POST}\n" && ${buffer} != "${mailAdd_GET}\n")
			{
				// check if mail address wants to be removed
				if (${buffer} != "${mailRemove_POST}\n" && ${buffer} != "${mailRemove_GET}\n")
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
		$mailtext = "Dear colleagues,\n\nthe Lunch Launcher has been launched!\n\n";
		foreach ($newtextArray as $newname => $newtext)
		{
			$mailtext = "${mailtext}$[newname} said: \"${newtext}\"\n";
		};
		$mailtext = "${mailtext}\n\nBon appetit,\n   The Lunch Launcher";

		// send the mail to the list
		foreach(preg_split("/((\r?\n)|(\r\n?))/", $maillist) as $maillistLine)
		{
			// the foreach gives an almost empty list... So make a check for '@'
			if (strpos ($maillistLine, '@') !== false)
			{
				mail ($maillistLine, "Lunch has been launched!", $mailtext);
			};
		};
	};
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
	<p>Add Mail: <input type="text" name="mailadd" value=""/></p>
	<p>Remove Mail: <input type="text" name="mailremove" value=""/></p>
	<p><input type="submit" value="Submit" /></p>
</form>

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

</body>
</html>
