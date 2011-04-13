<?php

?>

   <?php if ($picture) { print $picture; }?>
    
   <div class="node<?php if ($sticky) { print " sticky"; } ?><?php if (!$status) { print " node-unpublished"; } ?>">

   <?php if ($page == 0) { ?><h2 class="nodeTitle"><a href="<?php print $node_url?>"><?php print $title?></a>
	<?php global $base_url;
	if ($sticky) { print '<img src="'.base_path(). drupal_get_path('theme','sanqreal').'/img/sticky.gif" alt="sticky icon" class="sticky" />'; } ?>
	</h2><?php }; ?>
    
	<?php if (!$teaser): ?>
   	<?php if ($submitted): ?>
        <div class="metanode"><p><?php print t('') .'<span class="author">'. theme('username', $node).'</span>' . t(' - Posted on ') . '<span class="date">'.format_date($node->created, 'custom', "d F Y").'</span>'; ?></p></div>
      <?php endif;?>
      <div>
      <!-- tripal library theme -->
         <table>
            <tr><th>Unique Name</th><td><?php print $node->uniquename;?></td></tr>
            <tr><th>Organism</th><td><?php print $node->genus.' '.$node->species.' ('.$node->common_name.')';?></td></tr>
            <tr><th>Libraray Type</th><td><?php print $node->library_type?></td></tr>
            <!--<tr><th>Number of ESTs</th><td><?php print $node->sequence_num?></td></tr>-->
            <tr><th>Description</th><td><?php print $node->library_description?></td></tr>
         </table>
      <!-- End of tripal library theme-->
	  </div> 
    <?php endif; ?>
    
    <div class="content"><?php print $content?></div>
    
    <?php if (!$teaser): ?>
    <?php if ($links) { ?><div class="links"><?php print $links?></div><?php }; ?>
    <?php endif; ?>
    
    <?php if ($teaser): ?>
    <?php if ($links) { ?><div class="linksteaser"><div class="links"><?php print $links?></div></div><?php }; ?>
    <?php endif; ?>
    
    <?php if (!$teaser): ?>
    <?php if ($terms) { ?><div class="taxonomy"><span><?php print t('tags') ?></span> <?php print $terms?></div><?php } ?>
    <?php endif; ?>
  </div>

