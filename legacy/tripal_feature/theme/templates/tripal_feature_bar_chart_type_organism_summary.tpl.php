<?php
/**
 * Feature Summary Bar Chart (broken down by type & organism).
 *
 * This template displays a feature summary stacked bar chart
 * where each bar depicts the total number of features per
 * feature type and the stacked portions of a given bar
 * respresent the organism breakdown of those featurs.
 *
 * Most of the functionality is in the preprocess for this template and
 * the acompanying javascript file.
 *
 * @see tripal_feature/theme/js/tripalFeature.adminChart.js
 * @see tripal_feature/theme/tripal_feature.theme.inc:tripal_feature_preprocess_tripal_feature_bar_chart_type_organism_summary()
 */
?>

<div id="tripal-feature-admin-summary" class="tripal-admin-summary">
    <div id="tripal-feature-admin-summary-chart"
    "tripal-admin-chart">
</div>
<div id="tripal-feature-admin-summary-figure-desc" "tripal-admin-figure-desc">
<span class="figure-title">Feature Composition</span>:
This figure depicts the type and source organism of features in your Tripal
site. It is populated from the
<em><?php print $chart_details['mviewTable']; ?></em>
materialized view which was last updated on
<em><?php print $chart_details['mviewLastUpdate']; ?></em>.
<strong><em>To update this chart, <a
                href="<?php print $chart_details['mviewUrl']; ?>">
            submit a job to update the materialized view</a></em></strong>.
</div>
</div>

