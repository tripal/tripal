(function($) {
  Drupal.behaviors.tripal_ds = {
    attach: function (context, settings){
      // Add a close button for each pane except for the te_base
      $('div.tripal_pane').each(function (i) {
        if($(this).find('.tripal_pane-fieldset-buttons').length > 0) {
          return;
        }
        $(this).prepend(
          '<div class="tripal_pane-fieldset-buttons">' +
            '<div id="tripal-pane-close-button" class="tripal-pane-button">' +
              '<i class="fa fa-window-close-o fa-lg"></i>' +
            '</div>' +
          '</div>'
        );
      });

      // Hide the pane when the close button is clicked
      $('#tripal-pane-close-button .fa-lg').each(function (i) {
        $(this).click(function () {
          var fs = $(this).parents('div.tripal_pane');
          if($(fs).hasClass('showTripalPane'))  {
            $(fs).removeClass('showTripalPane');
            $(fs).hide('normal', function () {
              $(fs).addClass('hideTripalPane');
            });
          }
          else {
            $(fs).hide('normal', function () {
              $(fs).addClass('hideTripalPane');
              var id = $(fs).attr('id');
              var event = $.Event('tripal_ds_pane_collapsed', {
                id: id
              });
              $(id).trigger(event);
            });
          }
        });
      });

      // Move the tripal pane to the first position when its TOC item is clicked.
      $('.tripal_pane-toc-list-item-link').each(function (i) {
        var id = '.tripal_pane-fieldset-' + $(this).attr('id');
        if ($(id).length === 0) {
            $(this).parents('.views-row').first().remove();
            return;
        }
        $(this).click(function() {
          var prevObj = $(id).prev().attr('class');
          if(prevObj.length === 0) {
            return;
          }
          // Highlight the fieldset instead of moving if it's already at the top
          if (prevObj.indexOf('group-tripal-pane-content-top') == 0) {
            $(id).fadeTo(10, 0.3, function() {});
            $(id).fadeTo(200, 1, function() {});
          }
          if ($(id).hasClass('hideTripalPane')) {
            $(id).removeClass('hideTripalPane');
            $(id).addClass('showTripalPane');
          }
          $(id).hide();
          var obj = $(id).detach();
          $('.group-tripal-pane-content-top').after(obj);
          $(id).show(300, function () {
            // Trigger expansion event to allow the pane content
            // to react to the size change
            $(id).trigger($.Event('tripal_ds_pane_expanded', {id: id}));
            
            // Trigger a window resize event to notify charting modules that
            // the container dimensions has changed
            if (typeof Event !== 'undefined') {
              window.dispatchEvent(new Event('resize'));
            }
            else {
              // Support IE
              var event = window.document.createEvent('UIEvents');
              event.initUIEvent('resize', true, false, window, 0);
              window.dispatchEvent(event);
            }
          });
          return false;
        });
      });
    }
  };
})(jQuery);
