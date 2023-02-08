#pragma lint -1
#pragma optimize -1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gsos.h>
#include <resources.h>
#include <memory.h>


/*
 * This will DeRez icons and cursors from a resource file.
 *
 * Essentially "DeRez file Types.Rez" but with prettier output.
 *
 */


typedef struct IconHeader {
   Word iconType; // bit 15 = color (1) or b/w (0)
   Word iconSize; // number of bytes
   Word iconHeight; // height, in pixels
   Word iconWidth; // width, in pixels.
   // Byte iconImage[iconSize];
   // Byte iconMask[iconSize];
} IconHeader;

typedef struct CursorHeader {
   Word cursorHeight; // height, in pixels
   Word cursorWidth; // width, in pixels.
   // Byte cursorImage[height*width]
   // Byte cursorMask[height*width]
   // 
} CursorHeader;


typedef struct CursorTrailer {
   Word cursorY; // hotspot Y
   Word cursorX; // hotspot X.
   Word cursorFlags; // cursor ID
   // 8 bytes filler
} CursorTrailer;


GSString255Ptr c2gs(const char *cp) {
	GSString255Ptr gs;
	int l = strlen(cp);
	gs = malloc(l + 3);
	if (gs) {
		gs->length = l;
		strcpy(gs->text, cp);
	}
	return gs;
}


void dump(const unsigned char *ptr, unsigned count) {

	static char hex[] = "0123456789abcdef";

	unsigned i,x;
	fputs("    $\"", stdout);

	for (i = 0; i < count; ++i) {
		x = ptr[i];
		fputc(hex[x >> 4], stdout);
		fputc(hex[x & 0x0f], stdout);
	}

	fputs("\"\n", stdout);
}

void one_icon(ResID id, ResAttr attr, unsigned char *ptr, unsigned long length) {

	IconHeader *header;
	unsigned hh;
	unsigned ww;
	unsigned i;

	header = (IconHeader *)ptr;

	hh = header->iconHeight;
	ww = header->iconWidth >> 1;

	printf("resource rIcon(%lu, $%04x) {\n", id, attr);
	printf("    $%04x, // type (%s)\n", header->iconType, header->iconType & 0x8000 ? "Color" : "B/W");
	printf("    %u, // height\n", header->iconHeight);
	printf("    %u, // width\n\n", header->iconWidth);

	ptr += sizeof(IconHeader);
	// print the icon

	for (i = 0; i < hh; ++i) {
		dump(ptr, ww);
		ptr += ww;
	}

	printf("    ,\n");
	// print the mask

	for (i = 0; i < hh; ++i) {
		dump(ptr, ww);
		ptr += ww;
	}
	printf("};\n\n");
}


void one_cursor(ResID id, ResAttr attr, unsigned char *ptr, unsigned long length) {

	CursorHeader *header;
	CursorTrailer *trailer;
	unsigned hh;
	unsigned ww;
	unsigned i;

	header = (CursorHeader *)ptr;

	hh = header->cursorHeight;
	ww = (header->cursorWidth ) << 1;

	printf("resource rCursor(%lu, $%04x) {\n", id, attr);
	printf("    %u, // height\n", header->cursorHeight);
	printf("    %u, // width (%d bytes)\n\n", header->cursorWidth, ww << 1);

	ptr += sizeof(CursorHeader);
	// print the cursor

	for (i = 0; i < hh; ++i) {
		dump(ptr, ww);
		ptr += ww;
	}

	printf("    ,\n");
	// print the mask

	for (i = 0; i < hh; ++i) {
		dump(ptr, ww);
		ptr += ww;
	}
	printf("    ,\n");

	trailer = (CursorTrailer *)ptr;
	printf("    %u, // hotspot Y\n", trailer->cursorY);
	printf("    %u, // hotspot X\n\n", trailer->cursorX);
	printf("    $%02x // flags (%d mode)\n", trailer->cursorFlags, trailer->cursorFlags & 0x80 ? 640 : 320);
	printf("};\n\n");
}




void one_file(const char *name) {
	GSString255Ptr gname;
	unsigned rfd;
	unsigned depth;
	unsigned long ri;


	gname = c2gs(name);

	rfd = OpenResourceFile(0x8000 | readEnable, NULL, (Pointer)gname);
	if (_toolErr) {
		fprintf(stderr, "%s: OpenResourceFile: $%04x\n", name, _toolErr);
		free(gname);
		return;
	}

	depth = SetResourceFileDepth(1);

	// icons
	for (ri = 1; ; ++ri) {
		Handle h;
		ResAttr attr;
		ResID id = GetIndResource(rIcon, ri);
		if (_toolErr == resIndexRange) break;
		if (_toolErr) {
			fprintf(stderr, "%s: GetIndResource: $%04x\n", name, _toolErr);
			continue;
		}

		attr = GetResourceAttr(rIcon, id);
		h = LoadResource(rIcon, id);
		if (_toolErr) {
			fprintf(stderr, "%s: LoadResource: $%04x\n", name, _toolErr);
			continue;
		}

		HLock(h);
		one_icon(id, attr, *(char **)h, GetHandleSize(h));
		HUnlock(h);
		ReleaseResource(-1, rIcon, id);
	}

	// cursors
	for (ri = 1; ; ++ri) {
		Handle h;
		ResAttr attr;
		ResID id = GetIndResource(rCursor, ri);
		if (_toolErr == resIndexRange) break;
		if (_toolErr) {
			fprintf(stderr, "%s: GetIndResource: $%04x\n", name, _toolErr);
			continue;
		}

		attr = GetResourceAttr(rCursor, id);
		h = LoadResource(rCursor, id);
		if (_toolErr) {
			fprintf(stderr, "%s: LoadResource: $%04x\n", name, _toolErr);
			continue;
		}

		HLock(h);
		one_cursor(id, attr, *(char **)h, GetHandleSize(h));
		HUnlock(h);
		ReleaseResource(-1, rCursor, id);
	}



	SetResourceFileDepth(depth);
	CloseResourceFile(rfd);
	free(gname);
}

int main(int argc, char **argv) {

	int i;
	ResourceStartUp(MMStartUp());
	for (i = 1; i < argc; ++i) {
		one_file(argv[i]);
	}
	ResourceShutDown();
	exit(0);
}