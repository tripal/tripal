
(function ($) {

  Drupal.behaviors.chadoNodeApiFieldsetSummaries = {
    attach: function (context) {
    	
      // Properties Tab
      $('fieldset.chado-node-api.properties', context).drupalSetSummary(function (context) {
        return ChadoNodeApi_getSummary({
          machineName: {
            plural: 'properties',
            singular:'property'
          },
          readableName: {
            plural: 'properties',
            singular:'property'
          }
        });
      });

      // External References Tab
      $('fieldset.chado-node-api.dbxrefs', context).drupalSetSummary(function (context) {
        return ChadoNodeApi_getSummary({
          machineName: {
            plural: 'dbxrefs',
            singular:'dbxref'
          },
          readableName: {
            plural: 'references',
            singular:'reference'
          }
        });
      });
 
      // Relationships Tab
      $('fieldset.chado-node-api.relationships', context).drupalSetSummary(function (context) {
        return ChadoNodeApi_getSummary({
          machineName: {
            plural: 'relationships',
            singular:'relationship'
          },
          readableName: {
            plural: 'relationships',
            singular:'relationship'
          }
        });
      });

      function ChadoNodeApi_getSummary(api) {

        var numCurrent = $('tr.' + api.machineName.singular).length;
        var numOriginal = $('input.num-' + api.machineName.plural, context).val();
        var numSaved = $('tr.saved.' + api.machineName.singular).length;
        var numUnsaved = $('tr.unsaved.' + api.machineName.singular).length;
        var numRemoved = numOriginal - numSaved;


        // If there are no rows then tell the user that.
        if (numCurrent == 0) {
          if (numRemoved == 0) {
            return Drupal.t('No ' + api.readableName.plural);
          }
          else {
            return Drupal.t('No ' + api.readableName.plural + ' (<span class="chado-node-api removed">' + numRemoved + ' removed</span>)');
          }
        }
        // Otherwise, give them a breakdown of the current, new and removed rows
        // NOTE: Removed rows include only those that were original and have since been removed.
        else {
          var apiReadable = api.readableName.plural;
          if (numCurrent == 1) {
            apiReadable = api.readableName.singular;
          }

          if (numUnsaved != 0 && numRemoved != 0) {
            return Drupal.t(numCurrent + ' ' + apiReadable + ' (<span class="chado-node-api new">' + numUnsaved + ' new</span>; <span class="chado-node-api removed">' + numRemoved + ' removed</span>)');
          }
          else if (numRemoved != 0) {
            return Drupal.t(numCurrent + ' ' + apiReadable + ' (<span class="chado-node-api removed">' + numRemoved + ' removed</span>)');
          }
          else if (numUnsaved != 0) {
            return Drupal.t(numCurrent + ' ' + apiReadable + ' (<span class="chado-node-api new">' + numUnsaved + ' new</span>)');
          }
          else {
            return Drupal.t(numCurrent + ' ' + apiReadable);
          }
        }
      }
  }};
})(jQuery);