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

	// check if there are information in the url
	$newname = $_GET["name"];
	$newtext = $_GET["text"];
	if ($newname != "" && $newtext != "" )
	{
		$text = "${text}$timestamp;$newname;$newtext\n";
	};

	// write results to file
	file_put_contents ($file, $text, LOCK_EX);
?>

<h1>Lunch Launcher</h1>

<h2>Add Launch</h2>
<form action="index.php" method="post">
	
	<p>Name: <input type="text" name="name" value="<?php echo $setname; ?>"/></p>
 	<p>Launch: <input type="text" name="text" value="<?php echo $settext; ?>"/></p>
	<p><input type="submit" value="Submit" /></p>
</form>

<p></p>

<h2>Full Launch List - last <?php echo $minutes?> minutes</h2>
<?php
	$textprint = nl2br($text);
	$textprint = str_replace(array("\n","\r"), '', $textprint);
	echo $textprint;
?> 

<p></p>

<h2>Download latest script</h2>
<p>Latest stable version <a href="./lunchlauncher_skript.tar.gz">lunchlauncher_skript.tar.gz</a></p>
<p>Git Repository <a href="https://fortknox.physik3.gwdg.de/gitweb/?p=lunchlauncher.git;a=summary">lunchlauncher</a></p>

</body>
</html>
