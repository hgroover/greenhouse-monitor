<?php
// Poll and upload. Install this under some random name to make it harder to guess
// Get request
// $_SERVER['REQUEST_URI'] has the full path
$pathinfo = $_SERVER['PATH_INFO'];
if (preg_match('/\/poll\/(.+)/', $pathinfo, $m))
{
	header("Content-type: text/plain");
	// Do we have .data/0000n.req?
	$new_req = $m[1] + 1;
	$new_reqf = sprintf("%s/.data/%05d.req", realpath("."), $new_req);
	if (file_exists($new_reqf))
	{
		printf("%d\n", $new_req);
	}
	else
	{
		printf("%d %d rp %s\n", 0, $new_req, $new_reqf);
	}
}
else if (preg_match('/\/get\/(.+)/', $pathinfo, $m))
{
	header("Content-type: application/octet-stream");
	// Empty if non-existent
	$reqf = sprintf("%s/.data/%05d.req", realpath("."), $m[1]);
	if (file_exists($reqf))
	{
		// Passthrough
		readfile($reqf);
	}
	// else empty
}
else if (preg_match('/\/put\/(.+)/', $pathinfo, $m))
{
	header("Content-type: text/plain");
	// req_out contains script output
	// req_files[] may be an array of additional files uploaded
	$req_inf = sprintf("%s/.data/%05d.req", realpath("."), $m[1]);
	$req_log = sprintf("%s/.data/%05d.log", realpath("."), $m[1]);
	if (array_key_exists('req_out', $_FILES))
	{
		// Interested in name, size, tmp_name
		move_uploaded_file($_FILES['req_out']['tmp_name'], $req_log); // Returns true on success
	}
	if (array_key_exists('req_files', $_FILES))
	{
		$fn = 1;
		// Array with at least one
		foreach ($_FILES['req_files']['error'] as $key => $error)
		{
			$fname = sprintf("%s/.data/%05d-%d.dat", realpath("."), $m[1], $fn);
			$fn = $fn + 1;
			if ($error == UPLOAD_ERR_OK)
			{
				move_uploaded_file($_FILES['req_files']['tmp_name'][$key], $fname );
			}
		}
	}
}
else
{
	printf("err2:[%s]\n", $pathinfo);
}
