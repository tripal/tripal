(function($) {

  Drupal.behaviors.tripal_fields_layout = {
    attach: function (context, settings){
    
      // Add a close button for each panel
      $('.tripal_panel-fieldset .fieldset-legend').each(function (i) {
        $(this).append('<div class="tripal_panel-fieldset-close_button">[X]</div>');
      });
      // Hide the panel when the close button is clicked
      $('.tripal_panel-fieldset-close_button').each(function (i) {
    	$(this).css('float', 'right');
    	$(this).css('cursor', 'pointer');
    	$(this).css('margin', '0px 5px');
    	$(this).click(function () {
    		var fs = $(this).parent().parent().parent();
    		var fsid = fs.attr('id');
    		if (fsid.indexOf('tripal_panel-fieldset-') == 0) {
    		  $(fs).fadeOut(300);
    		}
    	});
      });
      
      // Move the panel to the first when its TOC item is clicked.
      $('.tripal_toc_list_item_link').each(function (i) {
    	  $(this).click(function() {
    		  var id = '#tripal_panel-fieldset-' + $(this).attr('id');
    		  $(id).removeClass('collapsed');
  		      $(id + ' .fieldset-wrapper').show();
    		  var prevObj = $(id).prev().attr('class');
    		  // Highlight the panel if it's already at the top
    		  if (prevObj.indexOf('tripal_panel-base_panel') == 0 && $(id).css('display') == 'block') {
    		    var color = $(id).css('background-color') ? $(id).css('background-color') : '#FFFFFF';
                    if (jQuery.ui) {
                      $(id).fadeTo(10, 0.5, function() {});
                      $(id).fadeTo(100, 1, function() {});
                      //$(id).effect('highlight', {color: '#DDDEEE'});
                    }
    		  }
    		  // Move the panel
    		  else {
    		    $(id).hide();
    		    var obj = $(id).detach();
    		    $('.tripal_panel-base_panel').after(obj);
    		    $(id).show(300);
    		  }
    		  return false;
    	  });
      });
    },
  };
  
})(jQuery);
