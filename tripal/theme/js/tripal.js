// Using the closure to map jQuery to $. 
(function ($) {
  // Store our function as a property of Drupal.behaviors.
  Drupal.behaviors.tripal = {
    attach: function (context, settings) {

      $(".tripal-entity-unattached .field-items").replaceWith('<div class="field-items">Loading... <img src="' + tripal_path + '/theme/images/ajax-loader.gif"></div>');
      $(".tripal-entity-unattached").each(function() {
        id = $(this).attr('id');
        if (id) {
          $.ajax({
            url: baseurl + '/bio_data/ajax/field_attach/' + id,
            dataType: 'json',
            type: 'GET',
            success: function(data){
              var content = data['content'];
              var id = data['id'];
              $("#" + id + ' .field-items').replaceWith(content);
            }
          });
        }
      });
    }
  }

})(jQuery);

// Used for ajax update of fields by links in a pager.
function tripal_navigate_field_pager(id, page) {
  jQuery(document).ajaxStart(function () {
    jQuery('#' + id + '-spinner').show();
  }).ajaxComplete(function () {
    jQuery('#' + id + '-spinner').hide();
  });

  jQuery.ajax({
    type: "GET",
    url: Drupal.settings["basePath"] + "bio_data/ajax/field_attach/" + id,
    data: { 'page' : page },
    success: function(response) {
      jQuery("#" + id + ' .field-items').replaceWith(response['content']);
    }
  });
}
