<?php
//require_once('../../../wp-blog-header.php');
require_once('../../../wp-load.php');
date_default_timezone_set ("Europe/Berlin");
  
//include only that one, rest required files will be included from it
include "phpqrcode/phpqrcode.php";
$margin = 0; // Default margin
$size=150;
$ecc = 0; // Default quality, values are L,M,Q,H
$data = '';
$bgcolor='ffffff';
$fgcolor='000000';
$logopercent=20;
$logo="default.png";
$token="";
if(isset($_REQUEST['bg'])) $bgcolor=$_REQUEST['bg'];
if(isset($_REQUEST['fg'])) $fgcolor=$_REQUEST['fg'];
if(isset($_REQUEST['logo'])) $logo=$_REQUEST['logo'];
if(isset($_REQUEST['lsize'])) $logopercent=$_REQUEST['lsize'];
if(isset($_REQUEST['token'])) $token=trim($_REQUEST['token']);
if(isset($_REQUEST['size']) && is_numeric($_REQUEST['size']))
  $size = $_REQUEST['size'];
if(isset($_REQUEST['margin']) && is_numeric($_REQUEST['margin']))
  $margin = $_REQUEST['margin'];
if(isset($_REQUEST['data'])) $data = $_REQUEST['data'];
if($size<64) $size = 64;
if($size>2000) $size = 2000;
if($margin<0) $margin=0;
if($margin>$size/3) $margin=$size/3;
if(isset($_REQUEST['ecc']) && is_numeric($_REQUEST['ecc'])) $ecc=$_REQUEST['ecc'];
if($ecc<0)
  $ecc = 0;
if($ecc>3)
  $ecc = 3;
if(strlen($data)==0) $data="No data entered!";

$protectdata = array("http://","mailto:","tel:","sms:","BEGIN:VCARD\nVERSION:2.1\n",
  "PHOTO;VALUE=URL:","FN:","ORG:","N:","TITLE:","ADR;","TEL;","EMAIL;WORK;INTERNET:",
  "EMAIL;HOME;INTERNET:","URL:","END:VCARD",
  "WORK;VOICE","HOME;VOICE","WORK;CELL","HOME;CELL","WORK;FAX","HOME;FAX",
  "\n",":",";",",");

$filename="";
$uploaddir = dirname(__FILE__).'/uploads/';
function startsWith($haystack, $needle)
{
    $length = strlen($needle);
    return (substr($haystack, 0, $length) === $needle);
}

function getcolor($img,$color,$rgb=true)
{
  if ($color[0] == '#')
        $color = substr($color, 1);
  if (strlen($color) == 6)
        list($r, $g, $b) = array($color[0].$color[1],
                                 $color[2].$color[3],
                                 $color[4].$color[5]);
  elseif (strlen($color) == 3)
        list($r, $g, $b) = array($color[0].$color[0], $color[1].$color[1], $color[2].$color[2]);
  else
        return false;
  $r = hexdec($r); $g = hexdec($g); $b = hexdec($b);
  if($rgb)
    return  array($r, $g, $b);
  return imagecolorallocate($img,$r,$g,$b);
}

//write code into file, Error corection lecer is lowest, L (one form: L,M,Q,H)
//each code square will be 4x4 pixels (4x zoom)
//code will have 2 code squares white boundary around 

//public static function png($text, $outfile = false, $level = QR_ECLEVEL_L, $size = 3, $margin = 4, $saveandprint=false) 
        
//QRcode::png($data, false, $ecc, $size, $margin);
$code = QRcode::image($data, $ecc, 1, 0);

$index = imagecolorexact( $code,  255,255,255 );
$bga = getcolor($code,$bgcolor);
imagecolorset($code,$index,$bga[0],$bga[1],$bga[2]);
$index = imagecolorexact( $code,0,0,0 );
$fga = getcolor($code,$fgcolor);
imagecolorset($code,$index,$fga[0],$fga[1],$fga[2]);

$w = imagesx($code);
$h = imagesy($code);
$img = imagecreatetruecolor($size,$size);
$bg = getcolor($img, $bgcolor,false);
$fg = getcolor($img,$fgcolor,false);
imagefilledrectangle ( $img,0,0,$size,$size,$bg);
imagecopyresized ($img,$code,$margin,$margin,0,0,$size-2*$margin,$size-2*$margin,$w,$h);

// insert logo

//$imglogo = imagecreatefrompng(plugins_url( 'uploads/'+$logo , __FILE__ ));
$imglogo = imagecreatefrompng('uploads/'.$logo);
$logow = imagesx($imglogo);
$logoh = imagesy($imglogo);
$lmax = max($logow,$logoh);
$logow = $logow*$size*$logopercent/(100*$lmax);
$logoh = $logoh*$size*$logopercent/(100*$lmax);
//echo "$logow $logoh $lmax $w";
if( imagecopyresampled ( $img ,$imglogo,intval($size/2-0.5*$logow), intval($size/2-0.5*$logoh),0,0,intval($logow) , intval($logoh) , imagesx($imglogo), imagesy($imglogo) )) {
  // Pay if necessary
}

header("Content-type: image/png");
imagepng($img);
imagecolordeallocate ($img,$bg);
imagedestroy($imglogo);
imagedestroy($img);
imagedestroy($code);
//flush();
exit();
?>                                