//
// Copyright 2009 Clemson University
//

if (Drupal.jsEnabled) {
   //------------------------------------------------------------
   // On document load we want to make sure the analysis result is shown
   var path = '';
   var tripal_AjaxRequests = 0;
   //--------------------------------------------
   function tripal_startAjax(){
      $("#tripal_ajaxLoading").show();
      tripal_AjaxRequests++;
   }
   //--------------------------------------------
   function tripal_stopAjax(){
      tripal_AjaxRequests--;
      if(tripal_AjaxRequests == 0){
         $("#tripal_ajaxLoading").hide();
      }
   }

   //------------------------------------------------------------
   // On document load we want to make sure that the expandable boxes
   // are closed
   $(document).ready(function(){
      // setup the expandable boxes used for showing blast results
      tripal_set_dropable_box();
      // tripal_set_dropable_subbox();
      var selected = location.hash;
      if(selected.substring(0,1) == '#'){
         $('#' + selected.substring(1)).next().show();
         $('#' + selected.substring(1)).css("background", "#E1CFEA");
      }
      
      // hide the transparent ajax loading popup
      $("#tripal_ajaxLoading").hide();
   });

   //------------------------------------------------------------
   function tripal_set_dropable_box(){
      //$('.tripal_expandableBoxContent').hide();
      
      $('.tripal_expandableBox').hover(
         function() {
            $(this).css("text-decoration", "none");
            $(this).css("background-color", "#EEFFEE");
         } ,
         function() {
            $(this).css("text-decoration", "none");
            $(this).css("background-color", "#EEEEFF");
         }
      );
      $('.tripal_expandableBox').click(
         function() {
            $(this).next().slideToggle('fast',
            function(){               
               var icon_url = $(this).prev().css("background-image");             
               if($(this).css("display") == "none" ){
            	  var changed_icon_url = icon_url.replace(/arrow-up-48x48.png/,"arrow-down-48x48.png");
                  $(this).prev().css("background-image", changed_icon_url);
                  $(this).prev().css("background-repeat","no-repeat");
                  $(this).prev().css("background-position","top right");
               } else {
            	   var changed_icon_url = icon_url.replace(/arrow-down-48x48.png/,"arrow-up-48x48.png");
                  $(this).prev().css("background-image", changed_icon_url);
                  $(this).prev().css("background-repeat","no-repeat");
                  $(this).prev().css("background-position","top right");
               }
            });
         }
      );
   }
   // Toggle the tripal_expandableBox
   function toggleExpandableBoxes(){
	   var status = $('#tripal_expandableBox_toggle_button').html();
	   var icon_url = $('.tripal_expandableBox').css("background-image");
	   icon_url = icon_url.toString().match(/.+\//);
	   icon_up = icon_url + "arrow-up-48x48.png)";
	   icon_down = icon_url + "arrow-down-48x48.png)";
	   
	   if (status == '[-] Collapse All') {
		   $('.tripal_expandableBoxContent').hide();
		   $('.tripal_expandableBox').css("background-image", icon_down);
		   $('#tripal_expandableBox_toggle_button').html('[+] Expand All');
	   } else {
		   $('.tripal_expandableBoxContent').show();
		   $('.tripal_expandableBox').css("background-image", icon_up);
		   $('#tripal_expandableBox_toggle_button').html('[-] Collapse All');
	   }
   }
}
