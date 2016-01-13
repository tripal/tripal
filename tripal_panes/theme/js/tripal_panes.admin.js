/**
 * Override the field UI's default behavior
 * @param $
 */
(function($) {

  Drupal.behaviors.tripalPane = {
    attach: function (context, settings) {
    	Drupal.behaviors.fieldUIDisplayOverview = {};
    	rearrangeRegion ();
    }
  };
  
  function rearrangeRegion () {
	  // For each field, make sure the selected value matches the region where it resides
	  $('#field-display-overview tr.tabledrag-leaf').each(function () {
		  // ID
		  var id = $(this).attr('id');
		  // Get the region
		  var region = getRegion (this).attr('class');
		  var regex = /region-title region-(.+)-title/;
		  var match = regex.exec(region);
		  var region_id = match[1].replace('-', '_');
		  var select = $(this).find('div.form-item-fields-' + id + '-region select');
		  $(select).children().each(function() {
			  if ($(this).val() == region_id) {
				  $(this).attr('selected', 'true');
			  } else {
				  $(this).attr('selected', null);
			  }
		  });
	  });
  }
  
  function getRegion (field) {
	  var previous = $(field).prev();
	  var region = null;
	  if ($(previous).hasClass('region-title')) {
		  region =  previous;
	  } else {
		  region = getRegion (previous);
	  }
	  return region;
  }
})(jQuery);