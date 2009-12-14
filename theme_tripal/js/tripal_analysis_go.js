//
// Copyright 2009 Clemson University
//

if (Drupal.jsEnabled) {
   function tripal_analysis_go_org_charts(item){
      if(!item){
         $("#tripal_analysis_go_org_charts").html('');
         return false;
      }
       
      link = '/tripal_analysis_go_org_charts/' + item;
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
