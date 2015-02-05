<?php
/*
Plugin Name: QR Generator 4 All
Plugin URI: http://www.internetloesungen.com/en/qr-generator-4-all/
Description: Create and embed QR Code images, offer QR generator to your user.
Version: 2.0.0
Author: Internetloesungen.com
Author URI: http://www.internetloesungen.com/en/
License: GPLv2 or later
*/

function iscurlinstalled() {return (in_array('curl', get_loaded_extensions()));}
function qrgen4_plugin_init() {
  load_plugin_textdomain('qrgen4', false, dirname( plugin_basename( __FILE__ ) ) . '/languages/');
}

function qrgen4all_plugin_menu() {
	add_options_page( 'QR Code Generator 4 All', 'QR Generator 4 All', 'manage_options', 'qrgen4all-setup-menu', 'qrgen4all_plugin_options' );
//call register settings function
	add_action( 'admin_init', 'qrgen4all_register_settings' );
}
function qrgen4all_register_settings() {
	//register our settings
	register_setting( 'qrgen4-settings-group', 'qrgen4all_sitetoken' );
}

add_action( 'init', 'qrgen4_plugin_init' );
add_action( 'admin_menu', 'qrgen4all_plugin_menu' );
add_shortcode('qrgen4all', 'qrgen4all_qrcode_shortcode');
add_shortcode('qrgen4allEditor', 'qrgen4all_editor');
register_activation_hook( __FILE__, 'qrgen4all_install' );

function qrgen4all_install() {
  global $wpdb;
  $table_name = $wpdb->prefix."qrgen4all_uploads";
    $sql = "CREATE TABLE " . $table_name . " (
    id MEDIUMINT(9) AUTO_INCREMENT,
    created DATETIME NOT NULL,
    file VARCHAR(200) NOT NULL,
    PRIMARY KEY  (id)
    );";
/* CREATE TABLE `wordpress`.`wp_qrgen4all_uploads` (
  `id` INT NOT NULL,
  `created` DATETIME NOT NULL,
  `file` VARCHAR(255) NOT NULL,
  PRIMARY KEY (`id`))
ENGINE = MyISAM
DEFAULT CHARACTER SET = utf8;
 * */
    require_once(ABSPATH.'wp-admin/includes/upgrade.php');
    dbDelta($sql);
}

function qrgen4all_plugin_options() {
	if ( !current_user_can( 'manage_options' ) )  {
		wp_die( __( 'You do not have sufficient permissions to access this page.' ) );
	}
  $lang = get_bloginfo("language");
  if(strlen($lang)>2) $lang = substr($lang,0,2);	
  
	?>
<div class="wrap">
<h1>QR Code Generator 4 All</h1>
<h2><?php _e('Implementation','qrgen4')?></h2>
<h3><?php _e('QR Code Generator','qrgen4')?></h3>
<p><?php _e('To implement the free configurable QR Code Generator, you just need to embed the following code:','qrgen4')?></p>
<div style="font-family:Courier New;background:#ececec;padding:10px">
[qrgen4allEditor]
</div>
<p><?php _e('The above version uses the full available width. To limit the width or to add a margin, you can also add style parameter:','qrgen4')?></p>
<div style="font-family:Courier New;background:#ececec;padding:10px">
[qrgen4allEditor style="width:700px;margin:0 auto"]
</div>

<h3><?php _e('Dynamic QR Codes','qrgen4')?></h3>
<p><?php _e('If you want to include dynamic generated QR Codes on your site, you can add them with the following shortcode:','qrgen4')?></p>
<div style="font-family:Courier New;background:#ececec;padding:10px;margin-bottom:10px">
[qrgen4all data="http://www.internetloesungen.com" size="200" margin="4" fg="222222" bg="ffffa0" class="myfloating" style="float:right;margin:10px" alt="Go to internetloesungen.com" title="Go to internetloesungen.com"]
</div>
<table>
<tr><th style="text-align:left;padding-right:10px"><?php _e('Option','qrgen4')?></th><th style="text-align:left;padding-right:10px"><?php _e('Required','qrgen4')?></th>
<th style="text-align:left;"><?php _e('Description','qrgen4')?></th></tr>
<tr><td>data</td><td><?php _e('Yes','qrgen4')?></td><td><?php _e('Content of the QR Code','qrgen4')?></td></tr>
<tr><td>size</td><td><?php _e('No','qrgen4')?></td><td><?php _e('Image size in pixel - Default: 150','qrgen4')?></td></tr>
<tr><td>margin</td><td><?php _e('No','qrgen4')?></td><td><?php _e('Image border in pixel - Default: 0','qrgen4')?></td></tr>
<tr><td>fg</td><td><?php _e('No','qrgen4')?></td><td><?php _e('Pixel color - Default: ffffff','qrgen4')?></td></tr>
<tr><td>bg</td><td><?php _e('No','qrgen4')?></td><td><?php _e('Background color - Default: 000000','qrgen4')?></td></tr>
<tr><td>class</td><td><?php _e('No','qrgen4')?></td><td><?php _e('CSS class for the image','qrgen4')?></td></tr>
<tr><td>style</td><td><?php _e('No','qrgen4')?></td><td><?php _e('Style tag for the image','qrgen4')?></td></tr>
<tr><td>alt</td><td><?php _e('No','qrgen4')?></td><td><?php _e('Alt tag for the image','qrgen4')?></td></tr>
<tr><td>title</td><td><?php _e('No','qrgen4')?></td><td><?php _e('Title tag for the image','qrgen4')?></td></tr>
</table>

<p><strong><?php _e('Examples','qrgen4')?></strong></p>
<div style="font-family:Courier New;background:#ececec;padding:10px">
[qrgen4all data="Hello World!"]<br/>
[qrgen4all data="mailto:yourmail@yourdomain.com?subject=Your subject" size="200" margin="4" fg="222222" bg="ffffa0" class="myfloating" style="float:right;margin:10px" alt="Mail me" title="Mail me"]<br/>

</div>
<p><?php _e('If you have a high traffic site, you should save the generated QR Codes as images instead of using the dynamic codes, because generating them on the fly increases your server load.','qrgen4')?></p>
<h3><?php _e('Customize User Interface','qrgen4')?></h3>
<p><?php _e('<a href="http://www.internetloesungen.com/en/bedienoberflache-anpassen/" target="_blank">Here we show how to customize the design of the user interface.</a>','qrgen4')?></p>
<?php echo qrgen4all_editor("max-width:700px;margin:0 auto;"); ?>
</div>
<?php
}


function qrgen4all_qrcode_shortcode($attr) {
	extract(shortcode_atts(array('data' => '', 'margin' => '0', 'size' => '150', 'fg' => '000000','bg'=>'ffffff','style'=>'','alt'=>'','class'=>''), $attr));
	$uri = plugins_url( 'qr.php' , __FILE__ );
	$data = (empty($data) && $data !==0) ? $uri : strip_tags(trim($data));  
	$data = urlencode($data);
	$margin = (empty($margin) && $margin !==0) ? "" : strip_tags(trim($margin));        
	$size = (empty($size) && $size !==0) ? "2" : strip_tags(trim($size));
	$fg = (empty($fg) && $fg !==0) ? "" : strip_tags(trim($fg));
	$bg = (empty($bg) && $bg !==0) ? "" : strip_tags(trim($bg));
	$style = (empty($style) && $style !==0) ? "" : strip_tags(trim($style));
	$class = (empty($class) && $class !==0) ? "" : strip_tags(trim($class));
	$image = $uri."?margin=$margin&amp;size=$size&amp;fg=$fg&amp;bg=$bg&amp;data=$data";
	if ($class != "") { $class = ' class="' . $class . '"'; }
	if ($style != "") { $style = ' style="' . $style . '"'; }
	return "<img src=\"$image\" $class $style alt=\"$alt\"";

}
function qrgen4all_editor($attr) {
  $style="";
  ob_start();
  if(is_string($attr)) $style=$attr;else {
	  extract(shortcode_atts(array('style' => ''), $attr));
  	$style = (empty($style) && $style !==0) ? "" : strip_tags(trim($style));
	}
  $stoken = get_option('qrgen4all_sitetoken');
  $customtoken = get_option('qrgen4all_ctoken');
  if(strlen($stoken)>10) {
    $validuntil = get_option('qrgen4all_tsv');
    if($validuntil===FALSE || strlen($customtoken)<20 || strlen($customtoken)>21 || $validuntil<time()) { // Get new token
      $ch = curl_init();
      curl_setopt($ch, CURLOPT_HEADER, 0);
      curl_setopt($ch, CURLOPT_VERBOSE, 0);
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
      curl_setopt($ch, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible;)");
      curl_setopt($ch, CURLOPT_POST, true);
      curl_setopt($ch, CURLOPT_URL, 'http://www.internetloesungen.com/wpde/wp-content/plugins/qrgen4allpro/renew-sitetoken.php' );
      curl_setopt($ch, CURLOPT_POSTFIELDS,array("s"=>$stoken));
      $customtoken = curl_exec($ch);
      //$customtoken = file_get_contents('http://www.internetloesungen.com/wpde/wp-content/plugins/qrgen4allpro/renew-sitetoken.php?s='.urlencode($stoken));
      update_option('qrgen4all_ctoken',$customtoken);
      update_option('qrgen4all_tsv',(time()+12*3600));    
    }
  }
  wp_enqueue_script('jquery-ui-slider');
  wp_enqueue_script('jquery-ui-button');
  wp_enqueue_script('jquery-ui-dialog');
  wp_enqueue_script('qrgen4all-editor',plugins_url( 'editor.js' , __FILE__ ));
  wp_localize_script( 'qrgen4all-editor', 'qrgen', array(
	'saveQR' => __('Your final Custom-QR-Code (save with right click)','qrgen4'),
	'confirmTokenUse' => __( 'Are all data complete and correct? A once used QR-Token is spent after usage and can not be used again.','qrgen4' ),
	'noToken'=>__('No QR-Token for final generation entered!','qrgen4'),
	'saveTitle'=>__('Save with right mouse click','qrgen4'),
	'demoImage'=>__('This is a test image with included errors.','qrgen4')
  ) );
  wp_enqueue_script('qrgen4all-colorpicker',plugins_url( 'jquery-miniColors/jquery.miniColors.min.js' , __FILE__ ));
  wp_enqueue_script('qrgen4all-forms',plugins_url( 'jquery.form.js' , __FILE__ ));
  wp_enqueue_style('qrgen4all-css', plugins_url( 'qrgen4all.css' , __FILE__ ));
  wp_enqueue_style('qrgen4all-colorpicker', plugins_url( 'jquery-miniColors/jquery.miniColors.css' , __FILE__ ));
  wp_enqueue_style('qrgen4all-ui', plugins_url( 'ui/css/ui.css' , __FILE__ ));
  $uri = plugins_url( 'qr.php' , __FILE__ );
  $uripro = plugins_url( 'qrpro.php' , __FILE__ );
  $path = plugins_url('',__FILE__);
  //$prouri = "http://www.internetloesungen.com/wpde/wp-content/plugins/qrgen4allpro/";
  $uploaduri = plugins_url( 'upload.php' , __FILE__ );
  $lang = get_bloginfo("language");
  if(strlen($lang)>2) $lang = substr($lang,0,2);	
  $o="";
?>
<script type="text/javascript">window.qr4all_prourl="<?php echo $uripro ?>";</script>
  <div id="qrgen4_upload_dialogl" title="<?php _e('Upload Logo','qrgen4')?>" style="text-align:left;font-size:12px;line-height:18px;">
  <p><?php _e('Here you can upload images with .png, .gif or .jpg extension. Best choice is PNG with alpha transparency. The logo will be inserted in the middle of the Custom-QR-Code.','qrgen4')?></p>
	<p><form action="<?php echo $uploaduri ?>" id="qrgen4_upload_form" method="POST" enctype="multipart/form-data">
<?php _e('Your Logo Image','qrgen4')?>:<input type="file" name="image"/><br/>
<input type="submit" name="upload_btn" value="<?php _e('Upload','qrgen4')?>"/>
</form></p>
<div id="qrgen4_upload_form_res"></div>
</div>
<div id="qrgen4_cinfo_dialog" title="<?php _e('QR Codes and Custom-QR-Codes','qrgen4')?>" style="text-align:left">
<table style="width:100%"><tr><td style="text-align:center;font-weight:bold"><?php _e('QR Code','qrgen4')?><br/><img src="<?php echo $path."/qrinfo.png";?>" style="height:175px;width:175px;margin:10px;" alt="<?php _e('QR Code','qrgen4')?>" title="<?php _e('QR Code','qrgen4')?>" /></td>
<td style="text-align:center;font-weight:bold"><?php _e('Custom-QR-Code','qrgen4')?><br/><img src="<?php echo $path."/".__('qrinfo-en.png');?>" style="height:175px;width:175px;margin:10px" alt="<?php _e('Custom-QR-Code','qrgen4')?>" title="<?php _e('Custom-QR-Code','qrgen4')?>" /></td></tr></table>
<?php _e('<h2>QR Code</h2><p>QR Codes (Quick Response Codes) are coded graphics, that can be read by current smartphones and tablets with the appropriate software. They form a bridge between the real and the virtual world. Nowadays it\'s hard to find advertisements without this code. With one scan you are directed to a website, a product video or a download link in the App Store. Receive immediately information or the content you want, without having to type a link. Complete Business cards are imported within seconds. Even pre-formulated, pre-addressed mail and SMS as well as free text messages can be transmitted. With the enormous range of practical applications QR Codes are continuously gaining importance.</p><h2>Custom-QR-Code</h2><p>Here you can customize your QR Codes with your logo, a photo, a smiley or a short promotional text. In this way you create a recognition and give a personal touch to the code, so to stand out from other competitors.</p><p>Your graphic is positioned centrally and may cover a maximum of 30%, less is recommended in any case, so that old scanner can read this.</p><p>You can upload your logo in PNG, GIF and as JPEG. We recommend PNG because here you can use alpha channel transparency. You can adjust the size of the logo with a slider. You should test in any case directly on the screen using a smartphone, whether the generated code is still working or if there is too many image content covered.</p>','qrgen4');
if(!$customtoken)
  _e('<p>Important: For tests the content of the Custom-QR-Code is in some places illegible. To generate a Custom-QR-Code correctly, a QR-Token is required. You can get it for 4,99 €. You should use the QR-Token only when everything else suits you. The generated Custom-QR Code will be displayed on the website and we sent it by mail to the email address you entered by ordering. If you make a price comparison, you will find that this offer is extremely cheap. Typical prices for individualized QR Codes range from 50,- € to 100,- €.</p><p>The provider of the QR-Token is the company Hot-World GmbH & Co. KG. Payment is made securely through PayPal.</p>','qrgen4');
_e('<p>The use of Custom-QR-Codes is free of charge and royalty. You can use your generated Custom-QR-Codes for free for personal and commercial use as well as for printing.</p>','qrgen4');?>
</div>
<div style="<?php echo $style; ?>">
<fieldset style="border:1px solid #000;padding:10px" id="qrgen4_box">
    <legend>&nbsp;<?php _e('QR Code Generator 4 All','qrgen4')?>&nbsp;</legend>
    <div id="qrgen4_tabs">
<form id="qrgen4all" name="qrgen4all" action="<?php echo $uri ?>">
<div id="qrgen4_tabset" style="margin-bottom:10px">
<input type="radio" name="tp" id="qrgen4_tp1" value="1" checked="checked"/><label for="qrgen4_tp1"><?php _e('Text','qrgen4')?></label>
<input type="radio" name="tp" id="qrgen4_tp2" value="2" /><label for="qrgen4_tp2"><?php _e('URL','qrgen4')?></label>
<input type="radio" name="tp" id="qrgen4_tp3" value="3" /><label for="qrgen4_tp3"><?php _e('Email','qrgen4')?></label>
<input type="radio" name="tp" id="qrgen4_tp4" value="4" /><label for="qrgen4_tp4"><?php _e('Telephone','qrgen4')?></label>
<input type="radio" name="tp" id="qrgen4_tp5" value="5" /><label for="qrgen4_tp5"><?php _e('SMS','qrgen4')?></label>
<input type="radio" name="tp" id="qrgen4_tp6" value="6" /><label for="qrgen4_tp6"><?php _e('Business Card','qrgen4')?></label>
</div>
<div id="qrgen4_tab1">
<?php _e('Text','qrgen4')?>:<br/>
<textarea name="text" rows="4" id="qrgen4_text" style="width:95%" counter="#qrgen4_textcount" max="1000"></textarea>
<div><span id="qrgen4_textcount">1000</span> <?php _e('chars left','qrgen4')?></div>
</div>

<div id="qrgen4_tab2" style="display:none">
<?php _e('Link','qrgen4')?>:<br/>
<input name="urldata" id="qrgen4_urldata" style="width:95%" value="http://"/>
</div>

<div id="qrgen4_tab3" style="display:none">
<?php _e('Email','qrgen4')?>:<br/>
<input name="email" id="qrgen4_email" style="width:95%" value=""/><br/>
<?php _e('Subject','qrgen4')?>:<br/>
<input name="emailsubject" id="qrgen4_emailsubject" style="width:95%" value=""/><br/>
<?php _e('Message','qrgen4')?>:<br/>
<textarea name="emailtext" rows="4" id="qrgen4_emailtext" style="width:95%"></textarea>
</div>

<div id="qrgen4_tab4" style="display:none">
<?php _e('Telephone Number','qrgen4')?>:<br/>
<input name="tel" id="qrgen4_tel" style="width:95%" value=""/>
</div>

<div id="qrgen4_tab5" style="display:none">
<?php _e('Telephone Number','qrgen4')?>:<br/>
<input name="sms" id="qrgen4_sms" style="width:95%" value=""/>
<?php _e('SMS Message','qrgen4')?>:<br/>
<textarea name="smstext" rows="4" id="qrgen4_smstext" style="width:95%" counter="#qrgen4_smscount" max="160"></textarea>
<div><span id="qrgen4_smscount">160</span> <?php _e('chars left','qrgen4')?></div>
</div>

<div id="qrgen4_tab6" style="display:none">
<?php _e('Business Card','qrgen4')?>:<br/>
<span class="t40"><?php _e('Formatted Name','qrgen4')?>:</span><input name="v_FN" id="qrgen4_v_FN" type="text" size="32" maxlength="64" value=""/><br/>
<span class="t40"><?php _e('Company Name','qrgen4')?>:</span><input name="v_company" id="qrgen4_v_company" type="text"  size="32" maxlength="64" value=""/><br/>
<span class="t40"><?php _e('URL','qrgen4')?>:</span><input name="v_url" id="qrgen4_v_url" type="text"  size="32" maxlength="64" value=""/><br/>
<span class="t40"><?php _e('Title','qrgen4')?>:</span><input name="v_comptitle" id="qrgen4_v_comptitle" type="text" size="32" maxlength="64" value=""/><br/>
<span class="t40"><?php _e('Surname','qrgen4')?>:</span><input name="v_family" id="qrgen4_v_family" type="text" size="32" maxlength="64" value=""/><br/>
<span class="t40"><?php _e('First Name','qrgen4')?>:</span><input name="v_namegiven" id="qrgen4_v_namegiven" type="text" size="32" maxlength="64" value=""/><br/>
<span class="t40"><?php _e('Image URL','qrgen4')?>:</span><input name="v_photo" id="qrgen4_v_photo" type="text" size="32" value=""/><br/>
<span class="t40"><?php _e('Address Type','qrgen4')?>:</span><select name="v_adrtype" id="qrgen4_v_adrtype"><option value="WORK"><?php _e('Work','qrgen4')?></option><option value="HOME"><?php _e('Home','qrgen4')?></option></select><br/>
<span class="t40"><?php _e('Post Office','qrgen4')?>:</span><input name="v_poaddr" id="qrgen4_v_poaddr" type="text" size="32" maxlength="64" value=""/><br/>
<span class="t40"><?php _e('Extended Address','qrgen4')?>:</span><input name="v_extaddr" id="qrgen4_v_extaddr" type="text" size="32" maxlength="64" value=""/><br/>
<span class="t40"><?php _e('Street','qrgen4')?>:</span><input name="v_street" id="qrgen4_v_street" type="text" size="32" maxlength="128" value=""/><br/>
<span class="t40"><?php _e('Postal Code','qrgen4')?>:</span><input name="v_postal" id="qrgen4_v_postal" type="text" size="16" maxlength="32" value=""/><br/>
<span class="t40"><?php _e('City','qrgen4')?>:</span><input name="v_locality" id="qrgen4_v_locality" type="text" size="16" maxlength="32" value=""/><br/>
<span class="t40"><?php _e('Region','qrgen4')?>:</span><input name="v_region" id="qrgen4_v_region" type="text" size="16" maxlength="32" value=""/><br/>
<span class="t40"><?php _e('Country Name','qrgen4')?>:</span><input name="v_country" id="qrgen4_v_country" type="text" size="16" maxlength="32" value=""/><br/>
<span class="t40"><?php _e('Telephone #1','qrgen4')?>:</span>
<select name="v_phonetype_a" id="qrgen4_v_phonetype_a">
<option value="WORK;VOICE" selected="selected"><?php _e('Work','qrgen4')?></option>
<option value="HOME;VOICE"><?php _e('Home','qrgen4')?></option>
<option value="WORK;CELL"><?php _e('Mobile Work','qrgen4')?></option>
<option value="HOME;CELL"><?php _e('Mobile Private','qrgen4')?></option>
<option value="WORK;FAX"><?php _e('Fax Work','qrgen4')?></option>
<option value="HOME;FAX"><?php _e('Fax Home','qrgen4')?></option>
</select>
<input type="text" name="v_tel_a" id="qrgen4_v_tel_a" size="20" maxlength="40" value=""/><br/>
<span class="t40"><?php _e('Telephone #2','qrgen4')?>:</span>
<select name="v_phonetype_b" id="qrgen4_v_phonetype_b">
<option value="WORK;VOICE"><?php _e('Phone Work','qrgen4')?></option>
<option value="HOME;VOICE" selected="selected"><?php _e('Phone Home','qrgen4')?></option>
<option value="WORK;CELL"><?php _e('Mobile Work','qrgen4')?></option>
<option value="HOME;CELL"><?php _e('Mobile Private','qrgen4')?></option>
<option value="WORK;FAX"><?php _e('Fax Work','qrgen4')?></option>
<option value="HOME;FAX"><?php _e('Fax Home','qrgen4')?></option>
</select>
<input type="text" id="qrgen4_v_tel_b" name="v_tel_b" size="20" maxlength="40" value=""/><br/>
<span class="t40"><?php _e('Telephone #3','qrgen4')?>:</span>
<select name="v_phonetype_c" id="qrgen4_v_phonetype_c">
<option value="WORK;VOICE"><?php _e('Phone Work','qrgen4')?></option>
<option value="HOME;VOICE"><?php _e('Phone Home','qrgen4')?></option>
<option value="WORK;CELL"><?php _e('Mobile Work','qrgen4')?></option>
<option value="HOME;CELL" selected="selected"><?php _e('Mobile Private','qrgen4')?></option>
<option value="WORK;FAX"><?php _e('Fax Work','qrgen4')?></option>
<option value="HOME;FAX"><?php _e('Fax Home','qrgen4')?></option>
</select>
<input type="text" name="v_tel_c" id="qrgen4_v_tel_c" size="20" maxlength="40" value=""/><br/>
<span class="t40"><?php _e('Telephone #4','qrgen4')?>:</span>
<select name="v_phonetype_d" id="qrgen4_v_phonetype_d">
<option value="WORK;VOICE"><?php _e('Phone Work','qrgen4')?></option>
<option value="HOME;VOICE"><?php _e('Phone Home','qrgen4')?></option>
<option value="WORK;CELL"><?php _e('Mobile Work','qrgen4')?></option>
<option value="HOME;CELL"><?php _e('Mobile Private','qrgen4')?></option>
<option value="WORK;FAX" selected="selected"><?php _e('Fax Work','qrgen4')?></option>
<option value="HOME;FAX"><?php _e('Fax Home','qrgen4')?></option>
</select>
<input type="text" name="v_tel_d" id="qrgen4_v_tel_d" size="20" maxlength="40" value=""/><br/>
<span class="t40"><?php _e('Email (Work)','qrgen4')?>: </span><input type="text" name="v_email_work" id="qrgen4_v_email_work" size="32" maxlength="64" value=""/><br/>
<span class="t40"><?php _e('Email (Home)','qrgen4')?>: </span><input type="text" name="v_email_home" id="qrgen4_v_email_home" size="32" maxlength="64" value=""/><br/>
</div>

<div id="qrgen4_tab2" style="display:none">
<select name="urltype" id="qrgen4_urltype"><option value="http://"><?php _e('Link: http://','qrgen4')?></option>
<option value="https://"><?php _e('Link save: https://','qrgen4')?></option>
<option value="mailto:"><?php _e('Email: mailto:','qrgen4')?></option>
<option value="tel:"><?php _e('Telephone','qrgen4')?>:</option>
</select><input name="urldata" id="qrgen4_urldata" style="wdith:300px"/>
</div>
<hr style="background-color:#D0D0BF;border:0 none;height:1px;"/>
<div id="qrgen4_pro" style="margin-bottom:8px">
<?php if(iscurlinstalled()) { ?>
<input type="radio" name="pro" id="qrgen4_pro1" value="1" checked="checked"/><label for="qrgen4_pro1"><?php _e('Standard QR Code','qrgen4')?></label>
<input type="radio" name="pro" id="qrgen4_pro2" value="2"/><label for="qrgen4_pro2"><?php _e('Custom-QR-Code with your Logo','qrgen4')?></label>
<span id="qrgen4_showinfo" style="white-space:nowrap;cursor:pointer;margin-left:15px;font-weight:bold"><?php _e('QR Code Info','qrgen4')?></span>
<?php }  else echo '<input type="radio" name="pro" id="qrgen4_pro1" value="1" checked="checked" style="display:none"/>' ?>
</div>

<table style="width:100%">
<tr><td style="width:18ex"><?php _e('Size','qrgen4')?>: <span id="qrgen4_size_out">200</span> px</td><td><div id="qrgen4_size"/></div></td></tr>
<tr><td><?php _e('Border','qrgen4')?>: <span id="qrgen4_margin_out">0</span> px</td><td><div id="qrgen4_margin"></div></td></tr>
<tr><td><?php _e('Foreground&nbsp;Color','qrgen4')?>:</td><td><input type="text" id="qrgen4_fg" name="fg" size="8" value="#000000"/></td></tr>
<tr><td><?php _e('Background&nbsp;Color','qrgen4')?>:</td><td><input type="text" name="bg" id="qrgen4_bg" size="8" value="#FFFFFF"/></td></tr> 
<tr class="qrgen4_prodata" style="display:none"><td><?php _e('Logo','qrgen4')?>:</td><td><button id="grgen4_open_upload"><?php _e('Upload new Logo','qrgen4')?></button></td></tr>
<tr class="qrgen4_prodata" style="display:none"><td><?php _e('Logo&nbsp;Size','qrgen4')?>:&nbsp;<span id="qrgen4_logosize_out">40</span>%</td><td><div id="qrgen4_logosize"></div></td></tr>
<?php if($customtoken && $stoken) { ?>
<tr><td colspan="2"><input type="submit" value="<?php _e('Update','qrgen4')?>"/>
<input type="hidden" name="token" id="qrgen4_token" value="<?php echo $customtoken;?>"/></td></tr> 
<?php } else { ?>
<tr><td colspan="2"><input type="submit" value="<?php _e('Update','qrgen4')?>"/>  </td></tr> 
<?php } ?></table>
</form>
</div>
<div id="qrgen4_output" style="text-align:center;margin:8px"><img src="<?php echo $uri ?>?size=200&margin=0"/></div>
<div id="qrgen4_outputpro" style="text-align:center;margin:8px"></div>
<div style="width:100%;text-align:center;margin:10px 0"><a href="http://www.qrcustomizerpro.com" target="_blank" style="text-decoration:none">Powered by <strong>QR Customizer Pro</strong> - Try it now for free!<br/>Design examples made with QR Customizer Pro:<br/><img src="http://www.qrcustomizerpro.com/qr/qr1.gif" style="height:248px;width:398px;border:0;margin-top:10px" title="Designed with QR Customizer Pro" /></a></div> 
<div style="font-size:10px;line-height:12px"><?php _e('Private and commercial use and printing are allowed.<br/>&quot;QR Code&quot; is a registered trademark of <a href="http://www.denso-wave.com/en/adcd/" target="_blank" rel="nofollow">DENSO WAVE</a> INCORPORATED','qrgen4')?></div>
</fieldset></div>
<?php
  $o = ob_get_contents();
  ob_end_clean();
  return $o;
}

?>