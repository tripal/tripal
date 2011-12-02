

if (Drupal.jsEnabled) {


   $(document).ready(function() {
       // Select default GO analysis when available
       var selectbox = $('#edit-tripal-analysis-go-select');
       if(selectbox.length > 0){ 
    	   var option = document.getElementById("analysis_id_for_go_report");
    	   if (option) {
    		   var options = document.getElementsByTagName('option');
    		   var index = 0;
    		   for (index = 0; index < options.length; index ++) {
    			   if (options[index].value == option.value) {
    				   break;
    			   }
    		   }
    		   selectbox[0].selectedIndex = index;
    		   tripal_analysis_go_org_charts(option.value);
    	// Otherwise, show the first option by default
    	   } else {
    		   selectbox[0].selectedIndex = 1;
    		   tripal_analysis_go_org_charts(selectbox.val());
    	   }
       }
   });


   function tripal_analysis_go_org_charts(item){
      if(!item){
         $("#tripal_analysis_go_org_charts").html('');
         return false;
      }
      // Form the link for the following ajax call 
      baseurl = tripal_get_base_url();      
      var link = baseurl + '?q=tripal_analysis_go_org_charts/' + item;
      tripal_startAjax();
      $.ajax({
           url: link,
           dataType: 'json',
           type: 'POST',
           success: function(data){
             $("#tripal_analysis_go_org_charts").html(data[0]);
             tripal_cv_init_chart();
             tripal_cv_init_tree();
             tripal_stopAjax();
           }
      });
      return false;
   }
}
