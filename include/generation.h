namespace evolution {
	class generation {
		public:
			void cross(unsigned parrentA, unsigned parrentB);
			generation selection(representation::representation *reference, unsigned count);
	};
}