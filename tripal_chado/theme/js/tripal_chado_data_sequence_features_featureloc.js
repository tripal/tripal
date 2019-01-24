//https://cdn.rawgit.com/calipho-sib/feature-viewer/v1.0.0/examples/index.html


(function ($) {

    Drupal.behaviors.tripal_analysis_blast = {
        attach: function (context, settings) {

            /**
             * JS to add the feature viewer.
             */
            tripal_chado_sequence_features_feature_viewers(settings.children_draw_info);

            // Remove the jquery.ui override of our link theme:
            $(".ui-widget-content").removeClass('ui-widget-content')
        }
    };

    /**
     * Initializes the feature viewers on the page.
     */
    function tripal_chado_sequence_features_feature_viewers(features) {

        var residues = features.residues
        children = features.children
        Object.keys(children).forEach(function (key, index) {
            //Each child gets its own feature viewer
            var options = {
                showAxis: true,
                showSequence: true,
                brushActive: true,
                toolbar: true,
                bubbleHelp: true,
                zoomMax: 3
            }

            var fv = new FeatureViewer(residues, '#tripal_sequence_features_featureloc_viewer_' + index, options);
            subFeatures = children[key]
            Object.keys(subFeatures).forEach(function (sfKey, sfIndex) {
                subFeature = subFeatures[sfKey]
                fv.addFeature(subFeature)
            })
        })
    }
})(jQuery);