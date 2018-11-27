enum Species {
	SPECIES_ANTELOPE, SPECIES_BADGER, SPECIES_BAT, SPECIES_BEAR, SPECIES_BOAR, SPECIES_BULL, SPECIES_CHEETAH, SPECIES_COYOTE, SPECIES_CROCODILE, SPECIES_CROW,
	SPECIES_DEER, SPECIES_DONKEY, SPECIES_DRAGON, SPECIES_EAGLE, SPECIES_ELEPHANT, SPECIES_FERRET, SPECIES_FOX, SPECIES_FROG, SPECIES_GERMAN_SHEPHARD,
	SPECIES_GIRAFFE, SPECIES_GOAT, SPECIES_GORILLA, SPECIES_GRYPHON, SPECIES_HARE, SPECIES_HIPPO, SPECIES_HORSE, SPECIES_HUSKY, SPECIES_HYENA, SPECIES_KANGAROO,
	SPECIES_LEOPARD, SPECIES_LION, SPECIES_LYNX, SPECIES_MEERKAT, SPECIES_MOUSE, SPECIES_OTTER, SPECIES_PANDA, SPECIES_PANTHER, SPECIES_PEGASUS, SPECIES_PERSIAN_CAT,
	SPECIES_PIT_BULL, SPECIES_POSSUM, SPECIES_RABBIT, SPECIES_RACCOON, SPECIES_RAT, SPECIES_REINDEER, SPECIES_RHINO, SPECIES_ROOSTER, SPECIES_ROTTWEILER,
	SPECIES_SALAMANDER, SPECIES_SHEEP, SPECIES_SIAMESE_CAT, SPECIES_SKUNK, SPECIES_SQUIRREL, SPECIES_TIGER, SPECIES_TRICERATOPS, SPECIES_UNICORN,
	SPECIES_VELOCIRAPTOR, SPECIES_WOLF, SPECIES_ZEBRA,
	SPECIES_FINAL
};

const char *speciesStrings[] = {
	"antelope", "badger", "bat", "bear", "boar", "bull", "cheetah", "coyote", "crocodile", "crow",
	"deer", "donkey", "dragon", "eagle", "elephant", "ferret", "fox", "frog", "german shephard",
	"giraffe", "goat", "gorilla", "gryphon", "hare", "hippo", "horse", "husky", "hyena", "kangaroo",
	"leopard", "lion", "lynx", "meerkat", "mouse", "otter", "panda", "panther", "pegasus", "persian cat",
	"pit bull", "possum", "rabbit", "raccoon", "rat", "reindeer", "rhino", "rooster", "rottweiler",
	"salamander", "sheep", "siamese cat", "skunk", "squirrel", "tiger", "triceratops", "unicorn",
	"velociraptor", "wolf", "zebra"
};

const char *randomNames[] = {
	"Aaron", "Adam", "Adrian", "Aiden", "Alan", "Alex", "Albert", "Andrew", "Arthur", "Barry", "Barney", "Ben", "Brad", "Brandon", "Brett", "Carl", "Cecil", "Cedric", "Charlie", "Chris", "Colin", "Craig", "Damian", "Daniel", "David", "Dennis", "Dexter", "Dylan", "Edward", "Eric", "Ethan", "Evan", "Eugene", "Felix", "Frank", "Fred", "Gabriel", "Gary", "George", "Greg", "Harry", "Harvey", "Hector", "Henry", "Ian", "Isaac", "Ivan", "Jack", "Jacob", "Jake", "James", "Jason", "Jerry", "Jim", "Joey", "John", "Josh", "Justin", "Kim", "Kenneth", "Keith", "Kevin", "Lee", "Leon", "Louis", "Lucas", "Malcolm", "Mark", "Marty", "Matt", "Maurice", "Max", "Mike", "Nathan", "Nick", "Oliver", "Oscar", "Otto", "Owen", "Patrick", "Paul", "Peter", "Philip", "Ralph", "Raymond", "Rhys", "Richard", "Rick", "Roger", "Rudy", "Sam", "Scott", "Sebastian", "Stan", "Steven", "Terry", "Tim", "Titus", "Tom", "Tobias", "Tony", "Trevor", "Tyler", "Vincent", "Victor", "Wayne", "Will", "Zack", "Zeke"
};

void setSpeciesStats(Unit *unit) {
	unit->strength = 5;
	unit->agility = 5;
	unit->constitution = 5;
	unit->size = 5;
	unit->ballSize = 5;

	if (unit->species == SPECIES_ANTELOPE) {
		unit->strength      = 4;
		unit->agility       = 7;
		unit->constitution  = 3;
		unit->size          = 4;
	} else if (unit->species == SPECIES_BADGER) {
		unit->strength      = 6;
		unit->agility       = 6;
		unit->constitution  = 5;
		unit->size          = 3;
	} else if (unit->species == SPECIES_BAT) {
		unit->strength      = 2;
		unit->agility       = 8;
		unit->constitution  = 4;
		unit->size          = 2;
	} else if (unit->species == SPECIES_BEAR) {
		unit->strength      = 9;
		unit->agility       = 3;
		unit->constitution  = 8;
		unit->size          = 9;
	} else if (unit->species == SPECIES_BOAR) {
		unit->strength      = 7;
		unit->agility       = 2;
		unit->constitution  = 8;
		unit->size          = 6;
	} else if (unit->species == SPECIES_BULL) {
		unit->strength      = 8;
		unit->agility       = 4;
		unit->constitution  = 6;
		unit->size          = 7;
	} else if (unit->species == SPECIES_CHEETAH) {
		unit->strength      = 4;
		unit->agility       = 7;
		unit->constitution  = 4;
		unit->size          = 3;
	} else if (unit->species == SPECIES_COYOTE) {
		unit->strength      = 4;
		unit->agility       = 6;
		unit->constitution  = 3;
		unit->size          = 3;
	} else if (unit->species == SPECIES_CROCODILE) {
		unit->strength      = 8;
		unit->agility       = 3;
		unit->constitution  = 10;
		unit->size          = 7;
	} else if (unit->species == SPECIES_CROW) {
		unit->strength      = 2;
		unit->agility       = 9;
		unit->constitution  = 4;
		unit->size          = 2;
	} else if (unit->species == SPECIES_DEER) {
		unit->strength      = 3;
		unit->agility       = 6;
		unit->constitution  = 4;
		unit->size          = 3;
	} else if (unit->species == SPECIES_DONKEY) {
		unit->strength      = 4;
		unit->agility       = 4;
		unit->constitution  = 4;
		unit->size          = 4;
	} else if (unit->species == SPECIES_DRAGON) {
		unit->strength      = 9;
		unit->agility       = 3;
		unit->constitution  = 7;
		unit->size          = 9;
	} else if (unit->species == SPECIES_EAGLE) {
		unit->strength      = 3;
		unit->agility       = 7;
		unit->constitution  = 4;
		unit->size          = 3;
	} else if (unit->species == SPECIES_ELEPHANT) {
		unit->strength      = 9;
		unit->agility       = 1;
		unit->constitution  = 6;
		unit->size          = 10;
	} else if (unit->species == SPECIES_FERRET) {
		unit->strength      = 2;
		unit->agility       = 10;
		unit->constitution  = 2;
		unit->size          = 1;
	} else if (unit->species == SPECIES_FOX) {
		unit->strength      = 4;
		unit->agility       = 9;
		unit->constitution  = 4;
		unit->size          = 2;
	} else if (unit->species == SPECIES_FROG) {
		unit->strength      = 1;
		unit->agility       = 10;
		unit->constitution  = 2;
		unit->size          = 1;
	} else if (unit->species == SPECIES_GERMAN_SHEPHARD) {
		unit->strength      = 3;
		unit->agility       = 5;
		unit->constitution  = 3;
		unit->size          = 3;
	} else if (unit->species == SPECIES_GIRAFFE) {
		unit->strength      = 5;
		unit->agility       = 4;
		unit->constitution  = 6;
		unit->size          = 7;
	} else if (unit->species == SPECIES_GOAT) {
		unit->strength      = 6;
		unit->agility       = 4;
		unit->constitution  = 7;
		unit->size          = 3;
	} else if (unit->species == SPECIES_GORILLA) {
		unit->strength      = 7;
		unit->agility       = 6;
		unit->constitution  = 7;
		unit->size          = 8;
	} else if (unit->species == SPECIES_GRYPHON) {
		unit->strength      = 6;
		unit->agility       = 7;
		unit->constitution  = 6;
		unit->size          = 4;
	} else if (unit->species == SPECIES_HARE) {
		unit->strength      = 3;
		unit->agility       = 9;
		unit->constitution  = 2;
		unit->size          = 1;
	} else if (unit->species == SPECIES_HIPPO) {
		unit->strength      = 7;
		unit->agility       = 2;
		unit->constitution  = 9;
		unit->size          = 8;
	} else if (unit->species == SPECIES_HORSE) {
		unit->strength      = 5;
		unit->agility       = 5;
		unit->constitution  = 6;
		unit->size          = 6;
	} else if (unit->species == SPECIES_HUSKY) {
		unit->strength      = 4;
		unit->agility       = 6;
		unit->constitution  = 3;
		unit->size          = 3;
	} else if (unit->species == SPECIES_HYENA) {
		unit->strength      = 4;
		unit->agility       = 7;
		unit->constitution  = 2;
		unit->size          = 3;
	} else if (unit->species == SPECIES_KANGAROO) {
		unit->strength      = 6;
		unit->agility       = 6;
		unit->constitution  = 4;
		unit->size          = 4;
	} else if (unit->species == SPECIES_LEOPARD) {
		unit->strength      = 6;
		unit->agility       = 7;
		unit->constitution  = 5;
		unit->size          = 4;
	} else if (unit->species == SPECIES_LION) {
		unit->strength      = 7;
		unit->agility       = 6;
		unit->constitution  = 5;
		unit->size          = 5;
	} else if (unit->species == SPECIES_LYNX) {
		unit->strength      = 4;
		unit->agility       = 6;
		unit->constitution  = 4;
		unit->size          = 3;
	} else if (unit->species == SPECIES_MEERKAT) {
		unit->strength      = 1;
		unit->agility       = 9;
		unit->constitution  = 2;
		unit->size          = 2;
	} else if (unit->species == SPECIES_MOUSE) {
		unit->strength      = 1;
		unit->agility       = 10;
		unit->constitution  = 1;
		unit->size          = 1;
	} else if (unit->species == SPECIES_OTTER) {
		unit->strength      = 2;
		unit->agility       = 9;
		unit->constitution  = 2;
		unit->size          = 2;
	} else if (unit->species == SPECIES_PANDA) {
		unit->strength      = 7;
		unit->agility       = 3;
		unit->constitution  = 6;
		unit->size          = 7;
	} else if (unit->species == SPECIES_PANTHER) {
		unit->strength      = 6;
		unit->agility       = 6;
		unit->constitution  = 5;
		unit->size          = 6;
	} else if (unit->species == SPECIES_PEGASUS) {
		unit->strength      = 5;
		unit->agility       = 5;
		unit->constitution  = 6;
		unit->size          = 6;
	} else if (unit->species == SPECIES_PERSIAN_CAT) {
		unit->strength      = 3;
		unit->agility       = 7;
		unit->constitution  = 3;
		unit->size          = 2;
	} else if (unit->species == SPECIES_PIT_BULL) {
		unit->strength      = 6;
		unit->agility       = 4;
		unit->constitution  = 6;
		unit->size          = 3;
	} else if (unit->species == SPECIES_POSSUM) {
		unit->strength      = 2;
		unit->agility       = 8;
		unit->constitution  = 3;
		unit->size          = 2;
	} else if (unit->species == SPECIES_RABBIT) {
		unit->strength      = 2;
		unit->agility       = 9;
		unit->constitution  = 2;
		unit->size          = 1;
	} else if (unit->species == SPECIES_RACCOON) {
		unit->strength      = 3;
		unit->agility       = 8;
		unit->constitution  = 3;
		unit->size          = 2;
	} else if (unit->species == SPECIES_RAT) {
		unit->strength      = 1;
		unit->agility       = 9;
		unit->constitution  = 3;
		unit->size          = 1;
	} else if (unit->species == SPECIES_REINDEER) {
		unit->strength      = 5;
		unit->agility       = 5;
		unit->constitution  = 6;
		unit->size          = 6;
	} else if (unit->species == SPECIES_RHINO) {
		unit->strength      = 7;
		unit->agility       = 4;
		unit->constitution  = 7;
		unit->size          = 8;
	} else if (unit->species == SPECIES_ROOSTER) {
		unit->strength      = 4;
		unit->agility       = 6;
		unit->constitution  = 3;
		unit->size          = 2;
	} else if (unit->species == SPECIES_ROTTWEILER) {
		unit->strength      = 5;
		unit->agility       = 5;
		unit->constitution  = 4;
		unit->size          = 3;
	} else if (unit->species == SPECIES_SALAMANDER) {
		unit->strength      = 4;
		unit->agility       = 7;
		unit->constitution  = 4;
		unit->size          = 2;
	} else if (unit->species == SPECIES_SHEEP) {
		unit->strength      = 3;
		unit->agility       = 4;
		unit->constitution  = 2;
		unit->size          = 3;
	} else if (unit->species == SPECIES_SIAMESE_CAT) {
		unit->strength      = 2;
		unit->agility       = 7;
		unit->constitution  = 1;
		unit->size          = 2;
	} else if (unit->species == SPECIES_SKUNK) {
		unit->strength      = 2;
		unit->agility       = 7;
		unit->constitution  = 3;
		unit->size          = 1;
	} else if (unit->species == SPECIES_SQUIRREL) {
		unit->strength      = 2;
		unit->agility       = 9;
		unit->constitution  = 2;
		unit->size          = 1;
	} else if (unit->species == SPECIES_TIGER) {
		unit->strength      = 6;
		unit->agility       = 6;
		unit->constitution  = 5;
		unit->size          = 6;
	} else if (unit->species == SPECIES_TRICERATOPS) {
		unit->strength      = 10;
		unit->agility       = 2;
		unit->constitution  = 8;
		unit->size          = 10;
	} else if (unit->species == SPECIES_UNICORN) {
		unit->strength      = 5;
		unit->agility       = 5;
		unit->constitution  = 6;
		unit->size          = 6;
	} else if (unit->species == SPECIES_VELOCIRAPTOR) {
		unit->strength      = 7;
		unit->agility       = 6;
		unit->constitution  = 9;
		unit->size          = 6;
	} else if (unit->species == SPECIES_WOLF) {
		unit->strength      = 7;
		unit->agility       = 6;
		unit->constitution  = 5;
		unit->size          = 6;
	} else if (unit->species == SPECIES_ZEBRA) {
		unit->strength      = 5;
		unit->agility       = 5;
		unit->constitution  = 6;
		unit->size          = 6;
	}
}
