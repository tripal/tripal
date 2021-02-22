// Using the closure to map jQuery to $. 
(function ($) {
  // Store our function as a property of Drupal.behaviors.
  Drupal.behaviors.so__transcript = {
    attach: function (context, settings) {

      $(".tripal-chado-so__transcript-box").hide()
      $(".tripal-chado-so__transcript-select").change(function() {
        $(".tripal-chado-so__transcript-box").hide()
        $("#tripal-chado-so__transcript-" + this.value).show(600)
      });
    }
  }
}) (jQuery);