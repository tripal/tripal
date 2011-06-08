<?php
// $Id: views-exposed-form.tpl.php,v 1.4.4.1 2009/11/18 20:37:58 merlinofchaos Exp $
/**
 * @file views-exposed-form.tpl.php
 *
 * This template handles the layout of the views exposed filter form.
 *
 * Variables available:
 * - $widgets: An array of exposed form widgets. Each widget contains:
 * - $widget->label: The visible label to print. May be optional.
 * - $widget->operator: The operator for the widget. May be optional.
 * - $widget->widget: The widget itself.
 * - $button: The submit button for the form.
 *
 * @ingroup views_templates
 */
?>
<?php if (!empty($q)): ?>
  <?php
    // This ensures that, if clean URLs are off, the 'q' is added first so that
    // it shows up first in the URL.
    print $q;

  ?>
<?php endif;     //dsm($widgets);?>
<div class="tripal_search_unigene-views-exposed-form views-exposed-form">
  <div class="views-exposed-widgets clear-block">
    
    <?php 
    	$feature_type_id =  $widgets['filter-feature_type']; 
    	$orgnism_common_name =  $widgets['filter-organism_common_name'];
    	$analysis_name =  $widgets['filter-unigene'];
    	$feature_name =  $widgets['filter-feature_name'];
    	$feature_seqlen =  $widgets['filter-feature_seqlen'];
    	$cvterm_name =  $widgets['filter-go_term'];
    	$blast_value =  $widgets['filter-blast_value'];
    	$interpro_value =  $widgets['filter-interpro_value'];
    	$kegg_value =  $widgets['filter-kegg_value'];
    ?>
    
    <div id="tripal-search-unigene-exposed-widgets">
    <div> Search ESTs or unigene contigs by name, assembly, sequence type, length, or their putative function. <br><br></div>
			<fieldset class="tripal-search-unigene-exposed-widgets-fields">
				<legend>Search by Name</legend>
				<div class="tripal-search-unigene-exposed-widget">
					<div class="tripal-search-unigene-form-labels">
						<label for="<?php print $orgnism_common_name->id; ?>"><?php print $orgnism_common_name->label; ?>
						</label>
					</div>
					<?php if (!empty($orgnism_common_name->operator)): ?>
					<div class="tripal_search_unigene-views-operator">
					<?php print $orgnism_common_name->operator; ?>
					</div>
					<?php endif; ?>
					<div class="tripal_search_unigene-views-widget">
					<?php print $orgnism_common_name->widget; ?>
					</div>
				</div>
				<div class="tripal-search-unigene-exposed-widget">
					<div class="tripal-search-unigene-form-labels">
						<label for="<?php print $feature_name->id; ?>"><?php print $feature_name->label; ?>
						</label>
					</div>
					<?php if (!empty($feature_name->operator)): ?>
					<div class="tripal_search_unigene-views-operator">
					<?php print $feature_name->operator; ?>
					</div>
					<?php endif; ?>
					<div class="tripal_search_unigene-views-widget">
						<?php print $feature_name->widget; ?>
					</div>
				</div>
			</fieldset>

			<fieldset class="tripal-search-unigene-exposed-widgets-fields">
				<legend>Search by Assembly</legend>
				<div class="tripal-search-unigene-exposed-widget">
					<div class="tripal-search-unigene-form-labels">
						<label for="<?php print $feature_type_id->id; ?>"><?php print $feature_type_id->label; ?>
						</label>
					</div>
					<?php if (!empty($feature_type_id->operator)): ?>
					<div class="tripal_search_unigene-views-operator">
					<?php print $feature_type_id->operator; ?>
					</div>
					<?php endif; ?>
					<div class="tripal_search_unigene-views-widget">
					<?php print $feature_type_id->widget; ?>
					</div>
				</div>
				<div class="tripal-search-unigene-exposed-widget">
					<div class="tripal-search-unigene-form-labels">
						<label for="<?php print $analysis_name->id; ?>"><?php print $analysis_name->label; ?>
						</label>
					</div>
					<?php if (!empty($analysis_name->operator)): ?>
					<div class="tripal_search_unigene-views-operator">
					<?php print $analysis_name->operator; ?>
					</div>
					<?php endif; ?>
					<div class="tripal_search_unigene-views-widget">
						<?php print $analysis_name->widget; ?>
					</div>
				</div>
			</fieldset>

			<fieldset class="tripal-search-unigene-exposed-widgets-fields">
				<legend>Search by Sequence</legend>
				<div class="tripal-search-unigene-exposed-widget">
					<div class="tripal-search-unigene-form-labels">
						<label for="<?php print $feature_seqlen->id; ?>"><?php print $feature_seqlen->label; ?>
						</label>
					</div>
					<?php if (!empty($feature_seqlen->operator)): ?>
					<div class="tripal_search_unigene-views-operator">
					<?php print $feature_seqlen->operator; ?>
					</div>
					<?php endif; ?>
					<div class="tripal_search_unigene-views-widget">
					<?php print $feature_seqlen->widget; ?>
					</div>
				</div>
			</fieldset>

			<fieldset class="tripal-search-unigene-exposed-widgets-fields">
				<legend>Search by Putative Function</legend>
				<div class="tripal-search-unigene-exposed-widget">
					<div class="tripal-search-unigene-form-labels">
						<label for="<?php print $cvterm_name->id; ?>"><?php print $cvterm_name->label; ?>
						</label>
					</div>
					<?php if (!empty($cvterm_name->operator)): ?>
					<div class="tripal_search_unigene-views-operator">
					<?php print $cvterm_name->operator; ?>
					</div>
					<?php endif; ?>
					<div class="tripal_search_unigene-views-widget">
					<?php print $cvterm_name->widget; ?>
					</div>
				</div>

				<div class="tripal-search-unigene-exposed-widget">
					<div class="tripal-search-unigene-form-labels">
						<label for="<?php print $blast_value->id; ?>"><?php print $blast_value->label; ?>
						</label>
					</div>
					<?php if (!empty($blast_value->operator)): ?>
					<div class="tripal_search_unigene-views-operator">
					<?php print $blast_value->operator; ?>
					</div>
					<?php endif; ?>
					<div class="tripal_search_unigene-views-widget">
						<?php print $blast_value->widget; ?>
					</div>
				</div>
				
				<div class="tripal-search-unigene-exposed-widget">
					<div class="tripal-search-unigene-form-labels">
						<label for="<?php print $kegg_value->id; ?>"><?php print $kegg_value->label; ?>
						</label>
					</div>
					<?php if (!empty($kegg_value->operator)): ?>
					<div class="tripal_search_unigene-views-operator">
					<?php print $kegg_value->operator; ?>
					</div>
					<?php endif; ?>
					<div class="tripal_search_unigene-views-widget">
						<?php print $kegg_value->widget; ?>
					</div>
				</div>
				
				<div class="tripal-search-unigene-exposed-widget">
					<div class="tripal-search-unigene-form-labels">
						<label for="<?php print $interpro_value->id; ?>"><?php print $interpro_value->label; ?>
						</label>
					</div>
					<?php if (!empty($interpro_value->operator)): ?>
					<div class="tripal_search_unigene-views-operator">
					<?php print $interpro_value->operator; ?>
					</div>
					<?php endif; ?>
					<div class="tripal_search_unigene-views-widget">
						<?php print $interpro_value->widget; ?>
					</div>
				</div>
			</fieldset>

		</div>
    
    <div class="tripal-search-unigene-exposed-widget">
    	<input TYPE="Button" value="Reset" onClick="window.location = '<?php global $base_url; print "$base_url/est_search"?>';">
      <?php $button = preg_replace("'Apply'", "Search", $button); print $button ?>
    </div>
  </div>
</div>