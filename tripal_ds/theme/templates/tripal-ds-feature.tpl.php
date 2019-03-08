<?php

/**
 * This template file provides the layout for the Tripal DS modules.
 *
 */
?>
<<?php print $layout_wrapper;
print $layout_attributes; ?> class="<?php print $classes; ?> clearfix">

<?php if (isset($title_suffix['contextual_links'])): ?>
  <?php print render($title_suffix['contextual_links']); ?>
<?php endif; ?>

<<?php print $left_wrapper ?> class="ds-left<?php print $left_classes; ?>">
<?php print $left; ?>
</<?php print $left_wrapper ?>>

<<?php print $right_wrapper ?> class="ds-right<?php print $right_classes; ?>">
<div class='group-tripal-pane-content-top'></div>
<?php print $right; ?>
</<?php print $right_wrapper ?>>

</<?php print $layout_wrapper ?>>
