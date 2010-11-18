//
// Copyright 2009 Clemson University
//

if (Drupal.jsEnabled) {
   
   $(document).ready(function(){
	   // If Anlaysis admin page is shown, get the settings for selected database
	   if ($("#edit-blastdb")[0]) {
		   tripal_update_regex($("#edit-blastdb")[0]);
		   tripal_set_genbank_style();
	   }
	   // Set blast hit alignment droppable box
	   tripal_set_blast_subbox();
   });
  
   //------------------------------------------------------------
   // Update the blast results based on the user selection
   function tripal_update_blast(link,db_id){
      tripal_startAjax();
      $.ajax({
         url: link.href,
         dataType: 'json',
         type: 'POST',
         success: function(data){         
            $("#blast_db_" + db_id).html(data.update);
            // make sure the newly added expandable boxes are closed
            tripal_set_blast_subbox(db_id);
            tripal_stopAjax();
         }
      });
      return false;
   }
   
   //------------------------------------------------------------
   // Update regular expression for selected database
   function tripal_update_regex(options){
	   // Get the dbname from DOM
	   var index = options.selectedIndex;
	   var dbid = options[index].value;

	   // Form the link for the following ajax call	   
      var baseurl = tripal_get_base_url();
      var link = baseurl + '/tripal_blast_regex/' + dbid;
	   
	   // Make ajax call to retrieve regular expressions
	   $.ajax( {
			url : link,
			dataType : 'json',
			type : 'POST',
			success : tripal_set_parser,
		});
	}
   
   // Set parser for the admin page
   function tripal_set_parser(data) {
	   // Set title if it exists
	   if (data.name) {
			$("#edit-displayname").val(data.name);
	   } else {
			$("#edit-displayname").val("");
	   }
		
	   // If genbank_style is TRUE, check the Genbank style box, clear all regular expressions, and disable
	   // the text fields
	   if (data.genbank_style == 1) {
		   $("#edit-gb-style-parser").attr("checked", true);
		   $("#edit-hit-id").val("");
		   $("#edit-hit-def").val("");
		   $("#edit-hit-accession").val("");
		
	   // Otherwise, uncheck the Genbank style box and set the regular expressions
	   } else {
			$("#edit-gb-style-parser").attr("checked", false);
			if (data.reg1) {
				$("#edit-hit-id").val(data.reg1);			
			// Show default hit-id parser if it's not set
			} else {
				$("#edit-hit-id").val("^(.*?)\s.*$");
			}
			if (data.reg2) {
				$("#edit-hit-def").val(data.reg2);
			// Show default hit-def parser if it's not set
			} else {
				$("#edit-hit-def").val("^.*?\s(.*)$");
			}
			if (data.reg3) {
				$("#edit-hit-accession").val(data.reg3);			
			// Show default hit-accession parser if it's not set
			} else {
				$("#edit-hit-accession").val("^(.*?)\s.*$");
			}
		}
		tripal_set_genbank_style();  
   }
   // ------------------------------------------------------------
   // Use genbank style parser. Hid regular expression text feilds
   function tripal_set_genbank_style (){
	  // Disable regular expressions if genbank style parser is used (checked)
	  if ($("#edit-gb-style-parser").is(":checked")) {
		  $("#edit-hit-id-wrapper > label").css("color", "grey");
		  $("#edit-hit-def-wrapper > label").css("color", "grey");
		  $("#edit-hit-accession-wrapper > label").css("color", "grey");
		  $("#edit-hit-id").attr('disabled', 'disabled');
		  $("#edit-hit-def").attr('disabled', 'disabled');
		  $("#edit-hit-accession").attr('disabled', 'disabled');
	  } else {
		  $("#edit-hit-id-wrapper > label").css("color", "black");
		  $("#edit-hit-def-wrapper > label").css("color", "black");
		  $("#edit-hit-accession-wrapper > label").css("color", "black");
		  $("#edit-hit-id").removeAttr('disabled');
		  $("#edit-hit-def").removeAttr('disabled');
		  $("#edit-hit-accession").removeAttr('disabled');
	  }
   }
   // -------------------------------------------------------------
   // Function that toggles the blast droppable subbox content
   function tripal_set_blast_subbox(db_id){
	  
	  $('.blast-hit-arrow-icon').hover(
	     function() {
	        $(this).css("cursor", "pointer");
	     },
	     function() {
	        $(this).css("cursor", "pointer");
	     }
	  );
	  if (!db_id){
		 $('.tripal_expandableSubBoxContent').hide();
	     $('.blast-hit-arrow-icon').click(
	        function() {
   	           // Find the width of the table column for the tripal_expandableSubBoxContent
	           var width = $(this).parent().parent().width();
	           width -= 40;
 	           // Traverse through html DOM objects to find tripal_expandableSubBoxContent and change its settings
	           var subbox = $(this).parent().parent().next().next().children().children();
	           subbox.css("width", width + 'px');
	           subbox.slideToggle('fast', function () {
	              var image = $(this).parent().parent().prev().prev().children().children();
	        	  var icon_url = image.attr("src");
	        	  if (subbox.is(':visible')) {
	        		 var changed_icon_url = icon_url.replace(/arrow_r.png/,"arrow_d.png");
	        		 image.attr("src", changed_icon_url);
	        	  } else {
	        		 var icon_url = icon_url.replace(/arrow_d.png/,"arrow_r.png");
	        		 image.attr("src", icon_url);
	        	  }
	           });
	        }
	     );
	  // Update only the part of DOM objects that have been changed by ajax. This is a solution
	  // to solve the problem that droppable subbox opened then closed immediately.
	  } else {
		  $("#blast_db_" + db_id + ' div.tripal_expandableSubBoxContent').hide();
		  var changedObject = $("#blast_db_" + db_id + " img.blast-hit-arrow-icon");
		  changedObject.click(
		     function() {
		        // Find the width of the table column for the tripal_expandableSubBoxContent
				var width = $(this).parent().parent().width();
			    width -= 40;
			    // Traverse through html DOM objects to find tripal_expandableSubBoxContent and change its settings
		        var subbox = $(this).parent().parent().next().next().children().children();
		        subbox.css("width", width + 'px');
		        subbox.slideToggle('fast', function () {
		        	var image = $(this).parent().parent().prev().prev().children().children();
		        	var icon_url = image.attr("src");
		        	if (subbox.is(':visible')) {
		        		var changed_icon_url = icon_url.replace(/arrow_r.png/,"arrow_d.png");
		        		image.attr("src", changed_icon_url);
		        	} else {
		        		var icon_url = icon_url.replace(/arrow_d.png/,"arrow_r.png");
		        		image.attr("src", icon_url);
		        	}
		        });
		     }
		  );
	  }
   }
}
