// Using the closure to map jQuery to $.
(function ($) {
  // Store our function as a property of Drupal.behaviors.
  Drupal.behaviors.tripal = {
    attach: function (context, settings) {

      $('.tripal-entity-unattached .field-items').replaceWith('<div class="field-items">Loading... <img src="' + tripal_path + '/theme/images/ajax-loader.gif"></div>');
      $('.tripal-entity-unattached').each(function () {
        var id = $(this).attr('id');
        if (id) {
          $.ajax({
            url     : baseurl + '/bio_data/ajax/field_attach/' + id,
            dataType: 'json',
            type    : 'GET',
            success : function (data) {
              var content = data['content'];
              var id      = data['id'];
              $('#' + id + ' .field-items').replaceWith(content);

              // If the field has no content, check to verify the pane is empty
              // then remove it.
              if (content.trim().length === 0) {
                var field   = $('#' + id);
                var classes = field.parents('.tripal_pane').first().attr('class').split(' ');
                var pane_id = null;

                // Remove the field since it's empty
                field.remove();

                // Get the tripal pane id to remove the pane if it is empty
                var sub_length = 'tripal_pane-fieldset-'.length;
                classes.map(function (cls) {
                  if (cls.indexOf('tripal_pane-fieldset-') > -1) {
                    pane_id = cls.substring(sub_length, cls.length);
                  }
                });

                if (pane_id) {
                  var pane = $('#' + pane_id);

                  // If the pane has only the title and close button, we can
                  // remove it
                  var has_children = $('.tripal_pane-fieldset-' + pane_id)
                    .first()
                    .children()
                    .not('.tripal_pane-fieldset-buttons')
                    .not('.field-group-format-title')
                    .not('#' + id).length > 0;

                  if (!has_children) {
                    pane.remove();
                  }
                }
              }
            }
          });
        }
      });
    }
  };

})(jQuery);

// Used for ajax update of fields by links in a pager.
function tripal_navigate_field_pager(id, page) {
  jQuery(document).ajaxStart(function () {
    jQuery('#' + id + '-spinner').show();
  }).ajaxComplete(function () {
    jQuery('#' + id + '-spinner').hide();
  });

  jQuery.ajax({
    type   : 'GET',
    url    : Drupal.settings['basePath'] + 'bio_data/ajax/field_attach/' + id,
    data   : {'page': page},
    success: function (response) {
      jQuery('#' + id + ' .field-items').replaceWith(response['content']);
    }
  });
}
