
#pragma once
struct Personality {
	/// <summary>
	/// 0 = defensive, 100 = aggresive, defensive is more likely to avoid risks and prioritize defensive positions whereas aggresive is more likely to attack and make sacrifices
	/// </summary>
	int playStyleBias = 50;

	/// <summary>
	/// A more composed player is less likely to make a blunder when the board is not in their favor
	/// </summary>
	int composure = 50;

	/// <summary>
	/// A smarter player can consider more plies when calculating moves
	/// </summary>
	int intelligence = 50;

	/// <summary>
	/// A more strategic player will consider positional strength and board control
	/// </summary>
	int strategic = 50;
};

