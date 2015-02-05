<?php
//require('../../../wp-blog-header.php');
require_once('../../../wp-load.php');
$uploaddir = dirname(__FILE__).'/uploads/';
$info = pathinfo($_FILES['image']['name']);
$ext = strtolower($info['extension']); // get the extension of the file
$fname = time()."_".rand().".png";
$img=null;
if($ext=='jpg') {
  $img = imagecreatefromjpeg ($_FILES['image']['tmp_name'] );
} else if($ext=="png") {
  $img =  imagecreatefrompng ($_FILES['image']['tmp_name']);
} else if($ext=='gif') {
  $img =  imagecreatefromgif ($_FILES['image']['tmp_name']);
}
if($img==null) {
 echo "<script type=\"text/javascript\">alert(\"".__('Upload failed. No valid file format detected.').$ext."\");</script>";
} else {
  $iw = imagesx($img);
  $ih = imagesy($img);
  $imax = max($iw,$ih);
  if($imax>1000) {
    $fak = 1000.0/$imax;
    $niw = intval($iw*$fak);
    $nih = intval($ih*$fak);
  } else {
    $niw = $iw;
    $nih = $ih;  
  }
  $img2 = imagecreatetruecolor($niw,$nih);
  imagealphablending( $img2, false );
  imagesavealpha( $img2, true );
  imagecopyresampled ( $img2 ,$img,0,0,0,0,$niw,$nih,$iw,$ih);
  imagepng($img2,$uploaddir.$fname);
  imagedestroy($img);
  imagedestroy($img2); 
  echo "<script type=\"text/javascript\">qrgen4_set_logo('".$fname."');</script>";
  $table_name = $wpdb->prefix."qrgen4all_uploads";
  $wpdb->query("INSERT INTO ".$table_name." SET created=NOW(),file='".$fname."'"); 
  // Delete expired files
  $rows = $wpdb->get_results("SELECT id,file FROM ".$table_name." where created<DATE_SUB(NOW(),INTERVAL 1 day);");
  if ( $rows ) {
	  foreach ( $rows as $row ) {
	     unlink($uploaddir.$row->file);
	     $wpdb->query("DELETE FROM ".$table_name." WHERE id=".$row->id);
	  }
	}
}

?>
