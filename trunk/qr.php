<?php
//include only that one, rest required files will be included from it
include "phpqrcode/phpqrcode.php";
$margin = 0; // Default margin
$size=150;    // Default size
$data = '';
$bgcolor='ffffff';
$fgcolor='000000';
if(isset($_REQUEST['bg'])) $bgcolor=$_REQUEST['bg'];
if(isset($_REQUEST['fg'])) $fgcolor=$_REQUEST['fg'];
if(isset($_REQUEST['size']) && is_numeric($_REQUEST['size']))
  $size = $_REQUEST['size'];
if(isset($_REQUEST['margin']) && is_numeric($_REQUEST['margin']))
  $margin = $_REQUEST['margin'];
if(isset($_REQUEST['data'])) $data = $_REQUEST['data'];
if($size<64) $size = 64;
if($size>2000) $size = 2000;
if($margin<0) $margin=0;
if($margin>$size/3) $margin=$size/3;
if(strlen($data)==0) $data="No data entered!";

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

$code = QRcode::image($data, 0, 1, 0);

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

//echo "Size:$size, w=$w margin=$margin";
ob_start();
imagepng($img);
$imageContent = ob_get_contents();
ob_end_clean();
header("Content-type: image/png");
header("Content-Length: " . strlen($imageContent));
echo $imageContent; //imagepng($img);

//ImagePng($code);
imagecolordeallocate ($img,$bg);
imagedestroy($img);
imagedestroy($code);

?>                                