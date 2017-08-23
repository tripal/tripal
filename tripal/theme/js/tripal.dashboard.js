(function ($) {
  Drupal.behaviors.tripalDashboard = {
    attach: function (context, settings) {


    /**
     * Renders the graphs for the Admin Dashboard.
     *
     * id: the element id into which the charts will be drawn.
     * fData: The data to be rendered in graphs.
     */

    function barchart2(id, parent, data) {
        // Set aside 10 colors
        var c10 = d3.scale.category10();
        
        // Set some default margins.
        var m = [30, 100, 10, 120];
        
        // Set the width of the viewport to fit inside the block.
        var w = $(parent).width() - m[1] - m[3] - 50;
        // Set the height to be tall enough to read each legend and surrounding
        // margins.
        var h = (data.length * 30) - m[0] - m[2]

        var format = d3.format(",.0f");

        var x = d3.scale.linear().range([0, w]),
            y = d3.scale.ordinal().rangeRoundBands([0, h], .1);

        var xAxis = d3.svg.axis().scale(x).orient("top").tickSize(-h),
            yAxis = d3.svg.axis().scale(y).orient("left").tickSize(0);

        var svg = d3.select(id).append("svg")
            .attr("width", w + m[1] + m[3])
            .attr("height", h + m[0] + m[2])
            .append("g")
            .attr("transform", "translate(" + m[3] + "," + m[0] + ")");

            // Parse numbers, and sort by count.
            data.forEach(function(d) { d.count = +d.count; });
            data.sort(function(a, b) { return b.count - a.count; });

            // Set the scale domain.
            x.domain([0, d3.max(data, function(d) { return d.count; })]);
            y.domain(data.map(function(d) { return d.name; }));

            var bar = svg.selectAll("g.bar")
                .data(data)
                .enter().append("g")
                .attr("class", "bar")
                .attr("transform", function(d) { return "translate(0," + y(d.name) + ")"; });

            bar.append("rect")
                .attr("width", function(d) { return x(d.count); })
                .attr("height", y.rangeBand())
                .attr('fill', function(d, i) { return c10(i); });

            bar.append("text")
                .attr("class", "count")
                .attr("x", function(d) { return x(d.count); })
                .attr("y", y.rangeBand() / 2)
                .attr("dx", 50)
                .attr("dy", ".35em")
                .attr("text-anchor", "end")
                .text(function(d) { return format(d.count); });

            svg.append("g")
                .attr("class", "x axis")
                .call(xAxis);

            svg.append("g")
                .attr("class", "y axis")
                .call(yAxis);
      }

      // Now insert the bar chart.
      barchart2('#tripal-entity-type-chart', '#block-tripal-content-type-barchart', entityCountListing);
    }
  };
}) (jQuery);