

if (Drupal.jsEnabled) {


   $(document).ready(function() {
       // Select default GO analysis when available
       var selectbox = $('#edit-tripal-analysis-go-select');
       if(selectbox.length > 0){ 
          selectbox[0].selectedIndex = 1;
          tripal_analysis_go_org_charts(selectbox.val());
       }
   });


   function tripal_analysis_go_org_charts(item){
      if(!item){
         $("#tripal_analysis_go_org_charts").html('');
         return false;
      }
      // Form the link for the following ajax call       
      var link = baseurl + '/tripal_analysis_go_org_charts/' + item;
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
