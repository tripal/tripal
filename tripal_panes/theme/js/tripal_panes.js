(function($) {

  Drupal.behaviors.tripal_panes = {
    attach: function (context, settings){

      // Add a close button for each pane except for the te_base
      $('.tripal_pane-fieldset .fieldset-legend').each(function (i) {
        if ($(this).parent().parent().attr('id') != 'tripal_pane-fieldset-te_base') {
          $(this).append('<div class="tripal_pane-fieldset-close_button"><img src="' + panes_theme_dir + '/images/close_btn.png" id="tripal-panes-close-button" class="tripal-panes-button"></div>');
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

      // Move the pane to the first when its TOC item is clicked.
      $('.tripal_panes-toc-list-item-link').each(function (i) {
        $(this).click(function() {
          var id = '#tripal_pane-fieldset-' + $(this).attr('id');

          var prevObj = $(id).prev().attr('class');
            // If the user clicks on the te_base TOC item, open the fieldset if it's closed
            if (id == '#tripal_pane-fieldset-te_base') {
              if ($('#tripal_pane-fieldset-te_base').hasClass('collapsed')) {
            	  $('#tripal_pane-fieldset-te_base').removeClass('collapsed');
            	  $('#tripal_pane-fieldset-te_base .fieldset-wrapper').show();
              }
              else {
                 $('#tripal_pane-fieldset-te_base').fadeTo(10, 0.3, function() {});
                 $('#tripal_pane-fieldset-te_base').fadeTo(200, 1, function() {});
              }
            }
            // If the user clicks on other TOC item, move its fieldset to the top right below the te_base
            else {
              $(id).removeClass('collapsed');
              $(id + ' .fieldset-wrapper').show();
              // Hightlight the fieldset instead of moving if it's already at the top
              if (prevObj.indexOf('tripal-panes-content-top') == 0 && $(id).css('display') == 'block' && $('#tripal_pane-fieldset-te_base').hasClass('collapsed')) {
                $(id).fadeTo(10, 0.3, function() {});
                $(id).fadeTo(200, 1, function() {});
              }
              else {
                $(id).hide();
                var obj = $(id).detach();
                $('.tripal-panes-content-top').after(obj);
              }
              // Close the te_base fieldset
              $('#tripal_pane-fieldset-te_base .fieldset-wrapper').hide();
              $('#tripal_pane-fieldset-te_base').addClass('collapsed');
            }
            $(id).show(300);
          //}
          return false;
        });
      });
    },
  };
  
})(jQuery);
