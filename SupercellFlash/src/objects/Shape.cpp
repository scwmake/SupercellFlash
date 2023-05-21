#include "SupercellFlash/objects/Shape.h"
#include "SupercellFlash/objects/ShapeDrawBitmapCommand.h"
#include "SupercellFlash/error/NullPointerException.h"
#include "SupercellFlash/error/NegativeTagLengthException.h"

#include "SupercellFlash/SupercellSWF.h"
#include "SupercellFlash/Tags.h"

namespace sc
{
	Shape* Shape::load(SupercellSWF* swf, uint8_t tag)
	{
		m_id = swf->stream.readUnsignedShort();

		uint16_t m_commandsCount = swf->stream.readUnsignedShort();
		commands = vector<pShapeDrawBitmapCommand>(m_commandsCount);

		if (tag == TAG_SHAPE_2)
			swf->stream.readUnsignedShort(); // total vertices count

		uint16_t commandsLoaded = 0;
		while (true)
		{
			uint8_t commandTag = swf->stream.readUnsignedByte();
			int32_t commandTagLength = swf->stream.readInt();

			if (commandTagLength < 0)
				throw NegativeTagLengthException(commandTag);

			if (commandTag == TAG_END)
				break;

			switch (commandTag)
			{
			case TAG_SHAPE_DRAW_BITMAP_COMMAND:
			case TAG_SHAPE_DRAW_BITMAP_COMMAND_2:
			case TAG_SHAPE_DRAW_BITMAP_COMMAND_3:
				commands[commandsLoaded] = pShapeDrawBitmapCommand((new ShapeDrawBitmapCommand())->load(swf, commandTag));
				commandsLoaded++;
				break;

			default:
				swf->stream.skip(commandTagLength);
				break;
			}
		}

		return this;
	}

	void Shape::save(SupercellSWF* swf)
	{
		uint32_t pos = swf->stream.initTag();

		uint16_t commandsCount = static_cast<uint16_t>(commands.size());

		swf->stream.writeUnsignedShort(m_id);
		swf->stream.writeUnsignedShort(commandsCount);

		uint8_t tag = TAG_SHAPE_2; // TODO: remove maxrects?

		for (const pShapeDrawBitmapCommand& command : commands) {
			if (command == nullptr) {
				throw NullPointerException<ShapeDrawBitmapCommand>();
			}

			for (const pShapeDrawBitmapCommandVertex& vert : command->vertices) {
				if (vert == nullptr) {
					throw NullPointerException<pShapeDrawBitmapCommandVertex>();
				}
			}
		}

		uint16_t totalVerticesCount = 0;
		for (uint16_t i = 0; commandsCount > i; i++) {
			totalVerticesCount += static_cast<uint16_t>(commands[i]->vertices.size());
		}

		if (TAG_SHAPE_2 == 18)
			swf->stream.writeUnsignedShort(totalVerticesCount);

		if (totalVerticesCount != 0) {
			for (uint16_t i = 0; commandsCount > i; i++) {
				commands[i]->save(swf, tag);
			}
		}

		// End tag
		swf->stream.writeTag(TAG_END);

		// TODO: tag 2 support
		swf->stream.finalizeTag(tag, pos);
	}
}