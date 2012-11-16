<?php
// Send image for custom code generation to internetloseungen 
//require('../../../wp-load.php');
$uploaddir = dirname(__FILE__).'/uploads/';
$info = pathinfo($_FILES['image']['name']);
$prouri = "http://www.internetloesungen.com/wpde/wp-content/plugins/qrgen4allpro/";
$uploaduri = $prouri.'upload.php';
$ext = strtolower($info['extension']); // get the extension of the file
$target = $uploaddir.time()."_".rand().'.'.$ext;

move_uploaded_file( $_FILES['image']['tmp_name'], $target);
$ch = curl_init();
curl_setopt($ch, CURLOPT_HEADER, 0);
curl_setopt($ch, CURLOPT_VERBOSE, 0);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible;)");
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_URL, $uploaduri );
curl_setopt($ch, CURLOPT_POSTFIELDS,array("image"=>"@".$target));
$response = curl_exec($ch);
echo $response;	
unlink($target);
?>