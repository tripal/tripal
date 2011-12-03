
if (Drupal.jsEnabled) {
   
   $(document).ready(function(){

   });
   //------------------------------------------------------------
   // Update the columns based on the chado table selection
   function tripal_update_chado_columns(select, url){
	   var link = url + "/admin/tripal/tripal_bulk_loader_template/add/chado_column/" + select.value;
	   $.ajax({
		   url: link,
		   dataType: 'json',
		   type: 'POST',
		   success: function(data){
			   var sel_col = $(select).parent().parent().next().children().children();
			   sel_col.attr("length", 0);
			   var options =  sel_col.attr('options');
			   for (var key in data) {
				   options [options.length] = new Option(data[key], key);
			   }
		   }
	   });
	   return false;
   }
}