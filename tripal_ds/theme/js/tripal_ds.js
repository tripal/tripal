(function($) {

  Drupal.behaviors.tripal_ds = {
    attach: function (context, settings){
    	console.log('tripal pane javascript');
      // Add a close button for each pane except for the te_base
      $('.tripal_pane-fieldset .fieldset-legend').each(function (i) {
        if ($(this).parent().parent().attr('id') != 'tripal_ds-fieldset-te_base') {
          $(this).append('<div class="tripal_pane-fieldset-close_button"><div id="tripal-pane-close-button" class="trip al-pane-button">CLOSE</div></div>');
        }
      });

      // Hide the pane when the close button is clicked
      $('.tripal_pane-fieldset-close_button').each(function (i) {
        $(this).css('float', 'right');
        $(this).css('cursor', 'pointer');
        $(this).css('margin', '0px 5px');
        $(this).click(function () {
          var fs = $(this).parent().parent().parent();
          var fsid = fs.attr('id');
          if (fsid.indexOf('tripal_pane-fieldset-') == 0) {
            $(fs).fadeOut(300);
          }
        });
      });

      // Move the tripal pane to the first position when its TOC item is clicked.
      $('.tripal_pane-toc-list-item-link').each(function (i) {
        $(this).click(function() {

          var id = '.tripal_pane-fieldset-' + $(this).attr('id');
          var prevObj = $(id).prev().attr('class');
          console.log("id: ");
          console.log(id);

          //console.log(prevObj);

            // If the user clicks on the tripal_pane-fieldset-group_summary_tripalpane TOC item, open the fieldset if it's closed
            if (id == '.tripal_pane-fieldset-group_summary_tripalpane') {
              if ($('.tripal_pane-fieldset-group_summary_tripalpane fieldset').hasClass('collapsed')) {
            	  $('.tripal_pane-fieldset-group_summary_tripalpane fieldset').removeClass('collapsed');
            	  $('.tripal_pane-fieldset-group_summary_tripalpane fieldset .fieldset-wrapper').show();
              }
              else {
                 $('.tripal_pane-fieldset-group_summary_tripalpane fieldset').fadeTo(10, 0.3, function() {});
                 $('.tripal_pane-fieldset-group_summary_tripalpane fieldset').fadeTo(200, 1, function() {});
              }
            }
            // If the user clicks on other TOC item, move its fieldset to the top right below the te_base
            else {
              $(id + ' fieldset').removeClass('collapsed');
              $(id + ' fieldset .fieldset-wrapper').show();
              // Hightlight the fieldset instead of moving if it's already at the top
              if (prevObj.indexOf('group-tripal-pane-content-top') == 0 && $('.tripal_pane-fieldset-group_summary_tripalpane fieldset').hasClass('collapsed')) {
                $(id + ' fieldset').fadeTo(10, 0.3, function() {});
                $(id + ' fieldset').fadeTo(200, 1, function() {});
              }
              else {
                $(id + ' fieldset .fieldset-wrapper').hide();
                var obj = $(id).detach();
                $('.group-tripal-pane-content-top').after(obj);
              }
              // Close the te_base fieldset
              $('.tripal_pane-fieldset-group_summary_tripalpane fieldset .fieldset-wrapper').hide();
              $('.tripal_pane-fieldset-group_summary_tripalpane fieldset').addClass('collapsed');
            }
            $(id + ' fieldset .fieldset-wrapper').show(300);
          //}
          return false;
        });
      });
    },
  };
  
})(jQuery);
