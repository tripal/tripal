if (Drupal.jsEnabled) {
  $(document).ready(function() {
    $('.tripal_feature-legend-item').bind("mouseover", function(){

        var classes = $(this).attr('class').split(" ");
        var type_class = classes[1];       
  
        $("." + type_class).css("border", "1px solid red");

        $(this).bind("mouseout", function(){
          $("." + type_class).css("border", "0px");
        })    
    });
  });
}
