<?php
$featureloc_sequences = $variables['tripal_feature']['featureloc_sequences'];

if(count($featureloc_sequences) > 0){
   foreach($featureloc_sequences as $src => $attrs){ ?>
       <div id="tripal_feature-<?php print $attrs['type']?>-box" class="tripal_feature-info-box tripal-info-box">
         <div class="tripal_feature-info-box-title tripal-info-box-title"><?php print $attrs['type']?> Formatted Sequence</div>
         <div class="tripal_feature-info-box-desc tripal-info-box-desc"></div>
            <?php print $attrs['formatted_seq'] ?>
       </div>       
   <?php } 
}?>

