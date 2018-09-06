// Using the closure to map jQuery to $.
(function ($) {
  // Store our function as a property of Drupal.behaviors.
  Drupal.behaviors.tripal = {
    attach: function (context, settings) {
      $('.tripal-entity-unattached .field-items').replaceWith('<div class="field-items">Loading... <img src="' + tripal_path + '/theme/images/ajax-loader.gif"></div>');
      $('.tripal-entity-unattached').each(function () {
        var id = $(this).attr('id');
        if (id) {
          var field = new AjaxField(id);
          field.load();
        }
      });
    }
  };

  /**
   * AjaxField Constructor.
   *
   * @param id
   * @constructor
   */
  function AjaxField(id) {
    this.id = id;

    this.hidePaneTitle();
  }

  /**
   * Hide pane title if the content of the pane has only ajax fields.
   */
  AjaxField.prototype.hidePaneTitle = function () {
    var id      = this.id;
    var field   = $('#' + id);
    var classes = field.parents('.tripal_pane').first().attr('class').split(' ');
    var pane_id = this.extractPaneID(classes);

    if (pane_id) {
      // Check if the fieldset has children that are not AJAX fields
      var has_children = $('.tripal_pane-fieldset-' + pane_id)
        .first()
        .children()
        .not('.tripal_pane-fieldset-buttons')
        .not('.field-group-format-title')
        .not('.tripal-entity-unattached')
        .not('#' + id).length > 0;

      // If there are no children, hide the pane title
      if (!has_children) {
        $('#' + pane_id).hide(0);
      }
    }
  };

  /**
   * Load the fields content from the server.
   */
  AjaxField.prototype.load = function () {
    $.ajax({
      url     : baseurl + '/bio_data/ajax/field_attach/' + this.id,
      dataType: 'json',
      type    : 'GET',
      success : this.handleSuccess.bind(this)
    });
  };

  /**
   * Add the content of the field to its pane.
   *
   * @param data
   */
  AjaxField.prototype.handleSuccess = function (data) {
    var content = data['content'];
    var id      = data['id'];
    var field   = $('#' + id);
    var classes = field.parents('.tripal_pane').first().attr('class').split(' ');
    var pane_id = this.extractPaneID(classes);

    $('#' + id + ' .field-items').replaceWith(content);

    // If the field has no content, check to verify the pane is empty
    // then remove it.
    if (content.trim().length === 0) {

      // Remove the field since it's empty
      field.remove();

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
    else {
      if (pane_id) {
        $('#' + pane_id).show(0);
      }
    }
  };

  /**
   * Extract the pane id from parent classes.
   *
   * @param classes
   * @return {String|null}
   */
  AjaxField.prototype.extractPaneID = function (classes) {
    var sub_length = 'tripal_pane-fieldset-'.length;
    var pane_id    = null;

    classes.map(function (cls) {
      if (cls.indexOf('tripal_pane-fieldset-') > -1) {
        pane_id = cls.substring(sub_length, cls.length);
      }
    });

    return pane_id;
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
