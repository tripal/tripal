// Using the closure to map jQuery to $.
(function ($) {
  // Store our function as a property of Drupal.behaviors.
  Drupal.behaviors.tripal = {
    attach: function (context, settings) {
    	
      // If we don't have any settings, this is not a entity page so exit
      if (!settings.tripal_display) {
        return;
      }

      // If the site does not support AJAX loading of fields then we're done.
      // Tripal will have already removed empty fields that wouldn't have 
      // needed AJAX loading.
      var use_ajax = settings.tripal_display.use_ajax;
      if (!use_ajax) {
        return;
      }

      // Attach all fields that require AJAX loading. 
      $('.tripal-entity-unattached .field-items').replaceWith('<div class="field-items">Loading... <img src="' + tripal_path + '/theme/images/ajax-loader.gif"></div>');
      $('.tripal-entity-unattached').each(function () {
        var id = $(this).attr('id');
        var hide_empty_field = settings.tripal_display.hide_empty;
        var field = new TripalAjaxField(id, hide_empty_field);
        field.attach();
      });
    }
  /*    else {
  if (pane_id) {
    $('#' + pane_id).show(0);
  }
*/
  };

  /* ------------------------------------------------------------------------
   *                        TripalPane Class
   * ------------------------------------------------------------------------
   */
  
  /**
   * TripalPane constructor
   */
  function TripalPane(id, hidden) {
    this.id = id;
    this.hidden = hidden;
  }
  
  /**
   * Indicates if the pane has any fields as chidren.
   */
  TripalPane.prototype.hasChildren = function() {
	var num_children = $('.tripal_pane-fieldset-' + this.id)
	  .first()
	  .children()
	  .not('.tripal_pane-fieldset-buttons')
	  .not('.field-group-format-title')
	  .not('#' + this.id)
	  .length > 0;
	  
	if (num_children > 0) {
      return true;
	}
	return false;
  }
  
  /**
   * Removes the pane from the HTML of the page.
   */
  TripalPane.prototype.remove = function() {
	// Remove the Pane's fieldset
	var pane = $('.tripal_pane-fieldset-' + this.id);
    pane.remove();
    
    // Remove the pane's title from the TOC.
    $('#' + this.id).parents('.views-row').remove();
  }
  
  /**
   * Removes a child from the pane.
   */
  TripalPane.prototype.removeChild = function(child_id) {
    var child = $('#' + child_id);
    
    // If this child is within a table then remove the row.
    var row = child.parents('tr');
    if (row) {
      row.remove();
    }
    
    child.remove();
  }
  
  /* ------------------------------------------------------------------------
   *                        TripalAjaxField Class
   * ------------------------------------------------------------------------
   */
  
  /**
   * TripalAjaxField Constructor.
   *
   * @param {Number} id
   * @param {Boolean} hide_fields
   * @constructor
   */
  function TripalAjaxField(id, hide_empty_field) {
    this.id = id;
    this.hide_empty_field = hide_empty_field;
    
    // Get the pane that this field beongs to (if one exists).
    this.pane = this.getPane(); 
  }

  /**
   * Load the field's content from the server.
   */
  TripalAjaxField.prototype.attach = function () {
    $.ajax({
      url     : baseurl + '/bio_data/ajax/field_attach/' + this.id,
      dataType: 'json',
      type    : 'GET',
      success : this.setFieldContent.bind(this)
    });
  };

  /**
   * Add the content of the field to its pane.
   *
   * @param data
   */
  TripalAjaxField.prototype.setFieldContent = function (data) {
	// Get the data items: the content, if this field is empty and the id 
	// of this field.
    var content = data['content'];
    var empty = data['is_empty'];
    var id = data['id'];
    
    // Get the field object.
    var field = $('#' + id);
    
    // First step, set the content for this field.  This will be the
    // field formatter content.
    $('#' + id + ' .field-items').replaceWith(content);

    // If the field is not empty then we're done.  Always show non-empty fields.
    if (!empty) {
      return;
    }
    
    // If empty fields should not be hidden then return.
    if (!this.hide_empty_field) {
      return;
    }
	
	// Second, if this field is part of a pane then we need to remove it
	// from the pane. Otherwise, just remove it.
	if (this.pane) {

      // Remove this field from the pane.
      this.pane.removeChild(id);
  
      // If the pane has no more children then remove it.
      if (!this.pane.hasChildren()) {
        this.pane.remove();
      }
	}
	else {
	  field.remove();
	}
  };

  /**
   * Extract the pane id from parent classes.
   *
   * @param classes
   * @return {String|null}
   */
  TripalAjaxField.prototype.getPane = function () {
	  
	// Get the pane for this field.
	var field = $('#' + this.id);
	var pane = field.parents('.tripal_pane')
	
	// If the field is not in a pane then just return.
	if (pane.length == 0) {
	  return null;
	}
	
	// Get further details about the pane.
	var classes = pane.first().attr('class').split(' ');
    var sub_length = 'tripal_pane-fieldset-'.length;
    var pane_id = null;

    classes.map(function (cls) {
      if (cls.indexOf('tripal_pane-fieldset-') > -1) {
        pane_id = cls.substring(sub_length, cls.length);
      }
    });
    
    if (pane_id) {
      var pane = new TripalPane(pane_id, false);
      return pane;
    }
    return null;
  };

})(jQuery);

// Used for ajax update of fields by links in a pager.
function tripal_navigate_field_pager(id, page) {

  jQuery('#' + id + '-spinner').show();
  jQuery.ajax({
    type   : 'GET',
    url    : Drupal.settings['basePath'] + 'bio_data/ajax/field_attach/' + id,
    data   : {'page': page},
    success: function (response) {
      jQuery('#' + id + ' .field-items').replaceWith(response['content']);
      jQuery('#' + id + '-spinner').hide();
    }
  });
}
