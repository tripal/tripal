
(function ($) {

  Drupal.behaviors.chadoNodeApiChangeNotify = {
    attach: function (context) {

    ChadoNodeApi_notifyChanges({
      machineName: {
        plural: 'properties',
        singular:'property'
      },
      readableName: {
        plural: 'properties',
        singular:'property'
      }
    });

    ChadoNodeApi_notifyChanges({
        machineName: {
          plural: 'dbxrefs',
          singular:'dbxref'
        },
        readableName: {
          plural: 'references',
          singular:'reference'
        }
      });

    ChadoNodeApi_notifyChanges({
        machineName: {
          plural: 'relationships',
          singular:'relationship'
        },
        readableName: {
          plural: 'relationships',
          singular:'relationship'
        }
      });
    
    function ChadoNodeApi_notifyChanges(api) {

      var numCurrent = $('tr.' + api.machineName.singular).length;
      var numOriginal = $('input.num-' + api.machineName.plural, context).val();
      var numSaved = $('tr.saved.' + api.machineName.singular).length;
      var numUnsaved = $('tr.unsaved.' + api.machineName.singular).length;
      var numRemoved = numOriginal - numSaved;

      // If changes have been made then notify the user.
      if (numUnsaved > 0 || numRemoved > 0) {
    	// Make the warning visible.
    	$('#' + api.machineName.singular + '-save-warning').css("display","inherit");

    	// Determine singular versus plural.
    	var unsavedReadable = api.readableName.plural;
        if (numUnsaved == 1) {
          unsavedReadable = api.readableName.singular;
        }
        var removedReadable = api.readableName.plural;
        if (numRemoved == 1) {
          removedReadable = api.readableName.singular;
        }
        
    	// Specify the changes made in the warning.
    	var note = '';
    	if (numUnsaved > 0 && numRemoved > 0) {
    		note = 'NOTE: Changes include the addition of ' + numUnsaved + ' ' + unsavedReadable + ' and the removal of ' + numRemoved + ' saved ' + removedReadable + '.';
    	}
    	else if (numUnsaved > 0) {
    		note = 'NOTE: Changes include the addition of ' + numUnsaved + ' ' + unsavedReadable + '.';
    	}
    	else if (numRemoved > 0) {
    		note = 'NOTE: Changes include the removal of ' + numRemoved + ' saved ' + removedReadable + '.';
    	}
    	$('#' + api.machineName.singular + '-save-warning span.specific-changes').html(note);
    	
    	// Add a * to any new records to make the warning more accessible.
    	$('tr.unsaved.' + api.machineName.singular + ' span.row-unsaved-warning').html('*');

      }
    }
  }};
})(jQuery);