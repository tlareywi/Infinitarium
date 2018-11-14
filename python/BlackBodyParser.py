from html.parser import HTMLParser

class BlackBodyParser(HTMLParser):
    current = ''
    data = []

    def handle_starttag( self, tag, attrs ):
        self.current = tag
    def handle_data(self, data):
        if( self.current != 'span' ):
            return

        tokens = data.split()
        if( tokens[2] != '2deg' ):
            return

        tuple = ( int(tokens[0]), (float(tokens[6]), float(tokens[7]), float(tokens[8])) )
        self.data.append( tuple )

    def handle_endtag( self, tag ):
        self.current = ''

    def KelvinToRGB( self, temp ):
        rgb = ()
        for t in self.data:
            rgb = t[1]
            if temp < t[0]:
                break

        return rgb

fh = open('/Users/trystan/Documents/bbr_color_D58.html', 'r')

parser = BlackBodyParser()

print('\n\nLoading black-body Kelvin to RGB mappings ...\n')
parser.feed( fh.read() )

fh.close()

# Unit tets
print( "Testing, 0 (out of range)", parser.KelvinToRGB(0) )
print( "Testing, 4000", parser.KelvinToRGB(4000) )
print( "Testing, 10000", parser.KelvinToRGB(10000) )
print( "Testing, 400000 (out of range)", parser.KelvinToRGB(400000), '\n' )
