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
else
{
	printf("err2:[%s]\n", $pathinfo);
}