if (Drupal.jsEnabled) {
   
   $(document).ready(function(){
	   var fontcolor = $(".interpro_results_table tbody tr td table tbody tr td b a").parent().prev().css('color');
	   // Disable the hyperlink for sequences in the interpro box
	   $(".tripal_interpro_results_table tbody tr td table tbody tr td b a").removeAttr('href');
	   $(".tripal_interpro_results_table tbody tr td table tbody tr td b a").css('font-weight','normal');
	   $(".tripal_interpro_results_table tbody tr td table tbody tr td b a").css('text-decoration','none');
	   $(".tripal_interpro_results_table tbody tr td table tbody tr td b a").css('color',fontcolor);
	   
	   // Allow selection of "Load GO term to the database" only when the submitting job to parse
	   // html output is enabled
	   isSubmittingJob ();
   });
   
   // Disable parse GO checkbox if no interpro job is submitted
   function isSubmittingJob () {
	   if ($('#edit-interprojob').is(":checked")) {
		   var fontcolor = $("#edit-parsego").parent().parent().prev().children().css('color');
		   $("#edit-parsego").attr('disabled', false);
		   $("#edit-parsego-wrapper").css("color", fontcolor);
		   
	   } else {
		   $("#edit-parsego").attr('checked', false);
		   $("#edit-parsego").attr('disabled', true);
		   $("#edit-parsego-wrapper").css("color", "grey");
	   }
   }
}
