//var qr4all_prourl = 'http://www.internetloesungen.com/wpde/wp-content/plugins/qrgen4allpro/qrpro.php';
var qrgen4all_timer = null;
var qrgen4all_logoname = 'default.png';
var qrgen4all_last_tp = 1;
var qrgen4all_last_cust = 1;

qrgen4all_vc_escape = function(text) {
  var l = text.length,i;
  var ascii = true;
  if(false && !ascii) {
    text = "ENCODING=QUOTED-PRINTABLE:"+qrgen4all_qprint(text);  
  }
  return text;
}

qrgen4all_update = function() {
  qrgen4all_update_token();
  return false;
}
qrgen4all_update_token = function() {
  qrgen4all_timer = null;
  var sel = jQuery('input[name=tp]:checked', '#qrgen4all').val();
  if(sel) qrgen4all_last_tp = sel; else sel = qrgen4all_last_tp;
  var data='';
  if(sel==1) data=jQuery('#qrgen4_text').val();
  if(sel==2) data=jQuery('#qrgen4_urldata').val(); 
  if(sel==3) {
    data='mailto:'+jQuery('#qrgen4_email').val();
    var c='?';
    var a=jQuery('#qrgen4_emailsubject').val();
    if(a.length>0) {data+=c+"subject="+encodeURIComponent(a);c='&';}
    a=jQuery('#qrgen4_emailtext').val();
    if(a.length>0) data+=c+"body="+encodeURIComponent(a);
  } 
  if(sel==4) data="tel:"+jQuery('#qrgen4_tel').val(); 
  if(sel==5) {
    data='sms:'+jQuery('#qrgen4_sms').val();
    var a=jQuery('#qrgen4_smstext').val();
    if(a.length>0) data+=":"+a;
  } 
  if(sel==6) {
    var b = jQuery('#qrgen4_tab6');
    data="BEGIN:VCARD\nVERSION:2.1\n";
    var x=jQuery('#qrgen4_v_FN').val();
    if(x.length>0)
      data+="FN:"+qrgen4all_vc_escape(x)+"\n";
    data+="N:"+qrgen4all_vc_escape(jQuery('#qrgen4_v_family').val())+';'+qrgen4all_vc_escape(jQuery('#qrgen4_v_namegiven').val())+';;;\n';
    x=jQuery('#qrgen4_v_company').val();      
    if(x.length>0) data+="ORG:"+qrgen4all_vc_escape(x)+"\n";
    x=jQuery('#qrgen4_v_comptitle').val();      
    if(x.length>0) data+="TITLE:"+qrgen4all_vc_escape(x)+"\n";
    x=jQuery('#qrgen4_v_photo').val();
    if(x.length>0) {
      data+="PHOTO;VALUE=URL:"+qrgen4all_vc_escape(x)+"\n";
    }
    data+="ADR;"+jQuery('#qrgen4_v_adrtype').val()+':'+qrgen4all_vc_escape(jQuery('#qrgen4_v_poaddr').val())+';'+qrgen4all_vc_escape(jQuery('#qrgen4_v_extaddr').val())+';'+
    qrgen4all_vc_escape(jQuery('#qrgen4_v_street').val())+';'+qrgen4all_vc_escape(jQuery('#qrgen4_v_locality').val())+';'+qrgen4all_vc_escape(jQuery('#qrgen4_v_region').val())+';'
    +qrgen4all_vc_escape(jQuery('#qrgen4_v_postal').val())+';'+qrgen4all_vc_escape(jQuery('#qrgen4_v_country').val())+"\n";
    x=jQuery('#qrgen4_v_tel_a').val();      
    if(x.length>0) data+="TEL;"+jQuery('#qrgen4_v_phonetype_a').val()+":"+qrgen4all_vc_escape(x)+"\n";
    x=jQuery('#qrgen4_v_tel_b').val();      
    if(x.length>0) data+="TEL;"+jQuery('#qrgen4_v_phonetype_b').val()+":"+qrgen4all_vc_escape(x)+"\n";
    x=jQuery('#qrgen4_v_tel_c').val();      
    if(x.length>0) data+="TEL;"+jQuery('#qrgen4_v_phonetype_c').val()+":"+qrgen4all_vc_escape(x)+"\n";
    x=jQuery('#qrgen4_v_tel_d').val();      
    if(x.length>0) data+="TEL;"+jQuery('#qrgen4_v_phonetype_d').val()+":"+qrgen4all_vc_escape(x)+"\n";
    x=jQuery('#qrgen4_v_email_work').val();      
    if(x.length>0) data+="EMAIL;WORK;INTERNET:"+qrgen4all_vc_escape(x)+"\n";
    x=jQuery('#qrgen4_v_email_home').val();      
    if(x.length>0) data+="EMAIL;HOME;INTERNET:"+qrgen4all_vc_escape(x)+"\n";
    x=jQuery('#qrgen4_v_url').val();      
    if(x.length>0) data+="URL:"+qrgen4all_vc_escape(x)+"\n";
    data+="END:VCARD";
  } 
  var pro = jQuery('input[name=pro]:checked', '#qrgen4all').val();
  if(pro) {pro = pro==2;qrgen4all_last_cust = pro;} else pro = qrgen4all_last_cust;
  var tosend;
  if(pro) {
    tosend = {
      size:jQuery('#qrgen4_size_out').html(), 
      margin:jQuery('#qrgen4_margin_out').html(),
      bg:jQuery('#qrgen4_bg').val(),
      fg:jQuery('#qrgen4_fg').val(),
      lsize:jQuery('#qrgen4_logosize_out').html(),
      logo:qrgen4all_logoname,
      ecc:3,
      data:data
    };
  } else {
    tosend = {
      size:jQuery('#qrgen4_size_out').html(), 
      margin:jQuery('#qrgen4_margin_out').html(),
      bg:jQuery('#qrgen4_bg').val(),
      fg:jQuery('#qrgen4_fg').val(),
      data:data
    };
  }
  var encoded = jQuery.param(tosend);
  var src = (pro ? qr4all_prourl : jQuery('#qrgen4all').attr('action'))+'?'+encoded;
  var mw = jQuery('#qrgen4_box').width()-20;
  if(jQuery('#qrgen4_size_out').html()<mw) mw=jQuery('#qrgen4_size_out').html();
  jQuery('#qrgen4_output').html("<img src=\""+src+"\" style=\"width:"+mw+"px;height:"+mw+"px\" title=\""+qrgen.saveTitle+"\"/>");
  return false;  
}
qrgen4all_select = function() {
  var sel = jQuery('input[name=tp]:checked', '#qrgen4all').val();
  if(sel==1) jQuery('#qrgen4_tab1').show(); else jQuery('#qrgen4_tab1').hide();
  if(sel==2) jQuery('#qrgen4_tab2').show(); else jQuery('#qrgen4_tab2').hide();
  if(sel==3) jQuery('#qrgen4_tab3').show(); else jQuery('#qrgen4_tab3').hide();
  if(sel==4) jQuery('#qrgen4_tab4').show(); else jQuery('#qrgen4_tab4').hide();
  if(sel==5) jQuery('#qrgen4_tab5').show(); else jQuery('#qrgen4_tab5').hide();
  if(sel==6) jQuery('#qrgen4_tab6').show(); else jQuery('#qrgen4_tab6').hide();
}
qrgen4all_selectpro = function() {
  var sel = jQuery('input[name=pro]:checked', '#qrgen4all').val();
  if(sel==2) jQuery('.qrgen4_prodata').show(); else jQuery('.qrgen4_prodata').hide();
}
qrgen4all_counter = function(event) {
 var obj = jQuery(event.target);
 max = obj.attr('max');
 var counter = obj.attr('counter');
 var len = obj.val().length;  
 if(len > max)
        obj.val(obj.val().substr(0, max));
 var text=(max-len);
 jQuery(counter).html(text);
}
qrgen4all_delayed_update = function() {
  if(qrgen4all_timer) clearInterval(qrgen4all_timer);
  qrgen4all_timer = setTimeout("qrgen4all_update()", 400);
}
qrgen4_uploadResponse = function() {
  jQuery('#qrgen4_upload_dialogl').dialog('close');
}

qrgen4_set_logo = function(logoname) {
  qrgen4all_logoname = logoname;
  qrgen4all_update();
}
jQuery(document).ready(function(){
  jQuery( "#qrgen4_upload_dialogl" ).dialog({height: 220,width:400,modal: true,autoOpen: false});
  jQuery('#grgen4_open_upload').button();
  jQuery('#grgen4_open_upload').click(function() {jQuery('#qrgen4_upload_dialogl').dialog('open');return false;});
  jQuery('#qrgen4_cinfo_dialog').dialog({height: 600,width:600,modal: true,autoOpen: false});
  jQuery('#qrgen4_showinfo').click(function() {jQuery('#qrgen4_cinfo_dialog').dialog('open');});
  jQuery('#qrgen4all').submit(qrgen4all_update);
  jQuery('input', '#qrgen4all').change(qrgen4all_update);
  jQuery('textarea', '#qrgen4all').change(qrgen4all_update);
  jQuery('input[name=tp]', '#qrgen4all').change(qrgen4all_select);
  jQuery('input[name=pro]', '#qrgen4all').change(qrgen4all_selectpro);
  jQuery('textarea[counter]', '#qrgen4all').keyup(qrgen4all_counter);
  jQuery('#qrgen4_fg').miniColors({
    change: function(hex, rgb) { qrgen4all_delayed_update(); }
  }); 
  jQuery('#qrgen4_bg').miniColors({
    change: function(hex, rgb) {qrgen4all_delayed_update(); }
  });
  jQuery("#qrgen4_tabset").buttonset();
  jQuery("#qrgen4_pro").buttonset();
  jQuery("#qrge4_coverage").buttonset();
  jQuery( "#qrgen4_size" ).slider({
			value:250,
			min: 60,
			max: 2000,
			step: 5,
			slide: function( event, ui ) {
				jQuery( "#qrgen4_size_out" ).html( ui.value );
				qrgen4all_delayed_update();
			}
	});
  jQuery( "#qrgen4_margin" ).slider({
			value:0,
			min: 0,
			max: 200,
			step: 1,
			slide: function( event, ui ) {
				jQuery( "#qrgen4_margin_out" ).html( ui.value );
				qrgen4all_delayed_update();
			}
	});
  jQuery( "#qrgen4_logosize" ).slider({
			value:40,
			min: 0,
			max: 100,
			step: 0.5,
			slide: function( event, ui ) {
				jQuery( "#qrgen4_logosize_out" ).html( ui.value );
				qrgen4all_delayed_update();
			}
	});
	// Image upload
  var options = { 
        target:'#qrgen4_upload_form_res', 
        success:qrgen4_uploadResponse 
  }; 
  jQuery('#qrgen4_upload_form').ajaxForm(options); 
});
