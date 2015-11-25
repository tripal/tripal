(function($) {

  Drupal.behaviors.tripal_fields_layout = {
    attach: function (context, settings){
    
      // Add a close button for each panel
      $('.tripal_panel_fieldset .fieldset-legend').each(function (i) {
        $(this).append('<div class="tripal_panel_fieldset-close_button">[X]</div>');
      });
      // Hide the panel when the close button is clicked
      $('.tripal_panel_fieldset-close_button').each(function (i) {
    	$(this).css('float', 'right');
    	$(this).css('cursor', 'pointer');
    	$(this).click(function () {
    		var fs = $(this).parent().parent().parent();
    		var fsid = fs.attr('id');
    		if (fsid.indexOf('tripal_panel_fieldset-') == 0) {
    		  $(fs).hide(300);
    		}
    	});
      });
      
      // Move the panel to the first when its TOC item is clicked.
      $('.tripal_toc_list_item_link').each(function (i) {
    	  $(this).click(function() {
    		  var id = '#tripal_panel_fieldset-' + $(this).attr('id');
    		  $(id).removeClass('collapsed');
  		      $(id + ' .fieldset-wrapper').show();
    		  var prevObj = $(id).prev().attr('class');
    		  // Highlight the panel if it's already at the top
    		  if (prevObj == 'tripal_base_panel' && $(id).css('display') == 'block') {
    			$(id).effect('highlight', {color: '#DDDEEE'});
    		  }
    		  // Move the panel
    		  else {
    		    $(id).hide();
    		    var obj = $(id).detach();
    		    $('.tripal_base_panel').after(obj);
    		    $(id).show(300);
    		  }
    		  return false;
    	  });
      });
    },
  };
  
})(jQuery);