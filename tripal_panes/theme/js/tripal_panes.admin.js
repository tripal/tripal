/**
 * Override the field UI's default behavior
 * @param $
 */
(function($) {

  Drupal.behaviors.tripalPane = {
    attach: function (context, settings) {
    	Drupal.behaviors.fieldUIDisplayOverview = {};
    }
  };
})(jQuery);