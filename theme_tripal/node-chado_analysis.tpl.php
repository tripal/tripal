<?php
//
// Copyright 2009 Clemson University
//
?>

   <?php if ($picture) {
      print $picture;
   }?>
    
   <div class="node<?php if ($sticky) { print " sticky"; } ?><?php if (!$status) { print " node-unpublished"; } ?>">

   <?php if ($page == 0) { ?><h2 class="nodeTitle"><a href="<?php print $node_url?>"><?php print $title?></a>
	<?php global $base_url;
	if ($sticky) { print '<img src="'.base_path(). drupal_get_path('theme','sanqreal').'/img/sticky.gif" alt="sticky icon" class="sticky" />'; } ?>
	</h2><?php }; ?>
    
	<?php if (!$teaser): ?>
	<?php if ($submitted): ?>
      <div class="metanode"><p><?php print t('') .'<span class="author">'. theme('username', $node).'</span>' . t(' - Posted on ') . '<span class="date">'.format_date($node->created, 'custom', "d F Y").'</span>'; ?></p></div>
      <div>
      <!-- tripal_analysis theme -->
         <table>
            <tr><th>Name</th><td><?php print $node->analysisname;?></td></tr>
            <tr><th>Program (version)</th><td><?php print $node->program.' ('.$node->programversion.')';?></td></tr>
            <?php
               $ver = $node->sourceversion;
               if ($node->sourceversion) {
                  $ver = "($node->sourceversion)";
               }
               $date = preg_replace("/^(\d+-\d+-\d+) .*/","$1",$node->timeexecuted);
            ?>
            <tr><th>Source (version)</th><td><?php print $node->sourcename.' '.$ver;?></td></tr>
            <tr><th>Source URI</th><td><?php print $node->sourceuri;?></td></tr>
            <tr><th>Executed</th><td><?php print $date?></td></tr>
            <tr><th>Description</th><td><?php print $node->description?></td></tr>
         </table>
      <!-- End of tripal_analysis theme-->
	  </div> 
    <?php endif; ?>
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
