// Using the closure to map jQuery to $. 
(function ($) {
  // Store our function as a property of Drupal.behaviors.
  Drupal.behaviors.myModuleSecureLink = {
    attach: function (context, settings) {

      $(".tripal-entity-unattached .field-items").replaceWith('Loading... <img src="' + tripal_path + '/theme/images/ajax-loader.gif">');
      $(".tripal-entity-unattached").each(function() {
        id = $(this).attr('id');
        $.ajax({
          url: baseurl + '/bio_data/ajax/field_attach/' + id,
          dataType: 'json',
          type: 'GET',
          success: function(data){
            var content = data['content'];
            var id = data['id'];
            $("#" + id).replaceWith(content);
          }
        });
      });
      
      $(".tripal-tooltip").each(function() {
        var field = $(this).next('.field');
        $(this).detach();

        if (field) {
          field.children('.field-label').append($(this));
        }
      });
    }
  }
})(jQuery);