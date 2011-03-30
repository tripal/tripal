

if (Drupal.jsEnabled) {


   $(document).ready(function() {
       // Select default GO analysis when available
       var selectbox = $('#edit-tripal-analysis-go-select');
       if(selectbox){ 
          selectbox[0].selectedIndex = 1;
          tripal_analysis_go_org_charts(selectbox.val());
       }
   });


   function tripal_analysis_go_org_charts(item){
      if(!item){
         $("#tripal_analysis_go_org_charts").html('');
         return false;
      }
      // Get the base url. Drupal can not pass it through the form so we need 
	   // to get it ourself. Use different patterns to match the url in case
      // the Clean URL function is turned on
      var baseurl = location.href.substring(0,location.href.lastIndexOf('/?q=/node'));
      if(!baseurl) {
   	   var baseurl = location.href.substring(0,location.href.lastIndexOf('/node'));
      }
      if (!baseurl) {
         // This base_url is obtained when Clena URL function is off
         var baseurl = location.href.substring(0,location.href.lastIndexOf('/?q=node'));
      }
      if (!baseurl) {
         // The last possibility is we've assigned an alias path, get base_url till the last /
         var baseurl = location.href.substring(0,location.href.lastIndexOf('/'));
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
