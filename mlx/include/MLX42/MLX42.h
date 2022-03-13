/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   MLX42.h                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: W2Wizard <w2.wizzard@gmail.com>              +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/12/28 00:33:01 by W2Wizard      #+#    #+#                 */
/*   Updated: 2022/03/09 13:44:52 by lde-la-h      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

/**
 * A cross-platform OpenGL graphics library based on the idea on what MiniLibX
 * provides. Just quite a bit better, in terms of code quality & performance.
 * 
 * As for the few void* present in some structs and functions and 
 * why MLX is split into two different headers, so to speak, 
 * is mainly for abstraction. Most users won't have a need for the inner 
 * workings of MLX (shaders, ...) and it also helps keep MLX nice and tidy.
 */

#ifndef MLX42_H
# define MLX42_H
# include <stdint.h>
# include <stdbool.h>
# include "MLX42_Keys.h"

/**
 * Base object for disk loaded textures.
 * It contains rudementary information about the texture.
 * 
 * @param width The width of the texture.
 * @param height The height of the texture.
 * @param bytes_per_pixel The amount of bytes in a pixel, always 4.
 * @param pixels The literal pixel data.
 */
typedef struct mlx_texture
{
	uint32_t	width;
	uint32_t	height;
	uint8_t*	pixels;
	uint8_t		bytes_per_pixel;
}	mlx_texture_t;

/**
 * Struct containing data regarding an XPM image.
 * 
 * @param texture The texture data of the XPM.
 * @param color_count The amount of colors available.
 * @param cpp The amount of characters per pixel.
 * @param mode The color mode, either (c)olor or (m)onochrome.
 */
typedef struct xpm
{
	mlx_texture_t	texture;
	int32_t			color_count;
	int32_t			cpp;
	char			mode;
}	xpm_t;

/**
 * An image instance can be summarized as just a simple
 * x, y & z coordinate.
 * 
 * Coordinates start from the top left of the screen at 0,0 and increase
 * towards the bottom right.
 * 
 * NOTE: To change the z value, use mlx_set_instance_depth!
 * 
 * @param x The x location.
 * @param y The y location.
 * @param z The z depth, controls if the image is on the fore or background.
 */
typedef struct mlx_instance
{
	int32_t	x;
	int32_t	y;
	int32_t	z;
}	mlx_instance_t;

/**
 * Key function callback data.
 * Data related the mlx_key_hook function
 * 
 * @param key The key that was pressed.
 * @param action The action that was done with the key.
 * @param os_key The os_key is unique for every key, and will have a 
 * different value/keycode depending on the platform. 
 * They may be consistent on different platforms.
 * @param modifier The modifier key that was pressed, 0 if none.
 */
typedef struct mlx_key_cbdata
{
	keys_t			key;
	action_t		action;
	int32_t			os_key;
	modifier_key_t	modifier;
}	mlx_key_data_t;

/**
 * An image with an individual buffer that can be rendered.
 * Any value can be modified except the width/height and context.
 * 
 * @param width The width of the image.
 * @param height The height of the image.
 * @param pixels The literal pixel data.
 * @param instances An instance carries the X, Y, Z location data.
 * @param count The element count of the instances array.
 * @param enabled If true the image is drawn onto the screen, else its not.
 * @param context Abstracted OpenGL data.
 */
typedef struct mlx_image
{
	const uint32_t	width;
	const uint32_t	height;
	uint8_t*		pixels;
	mlx_instance_t*	instances;
	int32_t			count;
	bool			enabled;
	void*			context;
}	mlx_image_t;

/**
 * Main MLX handle, carries important data in regards to the program.
 * @param window The window itself.
 * @param context Abstracted opengl data.
 * @param width The width of the window.
 * @param height The height of the window.
 * @param delta_time The time difference between the previous frame 
 * and the current frame.
 */
typedef struct mlx
{
	void*		window;
	void*		context;
	int32_t		width;
	int32_t		height;
	double		delta_time;
}	mlx_t;

// The error codes used to idenfity the correct error message.
typedef enum mlx_errno
{
	MLX_SUCCESS = 0,
	MLX_INVEXT,
	MLX_INVFILE,
	MLX_INVPNG,
	MLX_INVXPM,
	MLX_INVFONT,
	MLX_INVAREA,
	MLX_NULLARG,
	MLX_SHDRFAIL,
	MLX_MEMFAIL,
	MLX_GLADFAIL,
	MLX_GLFWFAIL,
	MLX_WINFAIL,
	MLX_IMGTOBIG,
	MLX_IMGTOSML,
	MLX_TEXTOBIG,
}	mlx_errno_t;

// Global error code from the MLX42 library, 0 on no error.
extern mlx_errno_t mlx_errno;

/**
 * Callback function used to handle scrolling.
 * 
 * @param[in] xdelta The mouse x delta.
 * @param[in] ydelta The mouse y delta.
 * @param[in] param Additional parameter to pass onto the function.
 */
typedef void (*mlx_scrollfunc)(double xdelta, double ydelta, void* param);

/**
 * Callback function used to handle key presses.
 * 
 * @param[in] keydata The callback data, contains info on key, action, ...
 * @param[in] param Additional parameter to pass onto the function.
 */
typedef void (*mlx_keyfunc)(mlx_key_data_t keydata, void* param);

/**
 * Callback function used to handle window resizing.
 * 
 * WARNING: The function is called every frame during which the window is being
 * resized, be aware!
 * 
 * @param[in] width The new width of the window.
 * @param[in] height The new height of the window. 
 * @param[in] param Additional parameter to pass onto the function.
 */
typedef void (*mlx_resizefunc)(int32_t width, int32_t height, void* param);

/**
 * Callback function used to handle window closing which is called when 
 * the user attempts to close the window, for example by clicking the 
 * close widget in the title bar.
 * 
 * @param[in] param Additional parameter to pass onto the function.
 */
typedef void (*mlx_closefunc)(void* param);

//= Error Functions =//

/**
 * Gets the english description of the error code.
 * 
 * @param val The error code.
 * @return The error string that describes the error code.
 */
const char* mlx_strerror(mlx_errno_t val);

//= Generic Functions =//

/**
 * Initilizes a new MLX42 Instance.
 * 
 * @param[in] width The width of the window.
 * @param[in] height The height of the window.
 * @param[in] title The title of the window.
 * @param[in] resize Enable window resizing.
 * @returns Ptr to the MLX handle or null on failure.
 */
mlx_t* mlx_init(int32_t width, int32_t height, const char* Tttle, bool resize);

/**
 * Notifies MLX that it should stop rendering and exit the main loop.
 * This is not the same as terminate, this simply tells MLX to close the window.
 * 
 * @param[in] mlx The MLX instance handle.
 */
void mlx_close_window(mlx_t* mlx);

/**
 * Initializes the rendering of MLX, this function won't return until
 * mlx_close_window is called, meaning it will loop until the user requests that
 * the window should close.
 * 
 * @param[in] mlx The MLX instance handle.
 */
void mlx_loop(mlx_t* mlx);

/**
 * Lets you set a custom image as the program icon.
 * 
 * NOTE: In MacOS this function does nothing, you should use the bundles icon to set the dock bar icon.
 * @link: https://9to5mac.com/2021/11/08/change-mac-icons/
 *
 * @param[in] mlx The MLX instance handle.
 * @param[in] image The image to use as icon.
 */
void mlx_set_icon(mlx_t* mlx, mlx_texture_t* image);

/**
 * Terminates MLX and cleans up any of its used resources.
 * Using any functions that require mlx afterwards will
 * be considered undefined behaviour, beware of segfaults.
 * 
 * @param[in] mlx The MLX instance handle.
 */
void mlx_terminate(mlx_t* mlx);

/**
 * Gets the elapsed time since MLX was initialized.
 * 
 * @return The amount of time elapsed in seconds.
 */
int32_t mlx_get_time(void);

//= Window/Monitor Functions

/**
 * This function brings the specified window to front and sets input focus.
 * 
 * Do not use this function to steal focus from other applications unless
 * you are certain that is what the user wants.  Focus stealing can be
 * extremely disruptive.
 * 
 * @param[in] mlx The MLX instance handle.
 */
void mlx_focus(mlx_t* mlx);

/**
 * Gets the size of the specified monitor.
 * 
 * @param[in] index Normally 0, incase of multiple windows, can be specified
 * @param[in] width The width of the window.
 * @param[in] height The height of the window.
 */
void mlx_get_monitor_size(int32_t index, int32_t* width, int32_t* height);

/**
 * Sets the windows position.
 * 
 *  Do not use this function to move an already visible window unless you
 *  have very good reasons for doing so, as it will confuse and annoy the user.
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] xpos The x position.
 * @param[in] ypos The y position.
 */
void mlx_set_window_pos(mlx_t* mlx, int32_t xpos, int32_t ypos);

/**
 * Gets the windows position.
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] xpos The x position.
 * @param[in] ypos The y position.
 */
void mlx_get_window_pos(mlx_t* mlx, int32_t* xpos, int32_t* ypos);

/**
 * Changes the window size to the newly specified values.
 * Use this to update the the window width and heigth values in the mlx ptr.
 * 
 * @param mlx The MLX instance handle.
 * @param new_width The new desired width.
 * @param new_height The new desired height.
 */
void mlx_set_window_size(mlx_t* mlx, int32_t new_width, int32_t new_height);

/**
 * Sets a desired min and max window width and height.
 * Will force the window to not be resizable past or below the given values.
 * Due to norme constraints this is a bit ugly, sorry.
 * 
 * @param mlx The MLX instance handle.
 * @param min_wh The min width and height values.
 * @param max_wh The max width and height values.
 */
void mlx_set_window_limit(mlx_t* mlx, int32_t min_w, int32_t min_h, int32_t max_w, int32_t max_h);

//= Input Functions =//

/**
 * Returns true or false if the key is down or not.
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] key The keycode to check, use MLX_KEY_... to specify!
 * @returns True or false if the key is down or not.
 */
bool mlx_is_key_down(mlx_t* mlx, keys_t key);

/**
 * Checks whether a mouse button is pressed or not.
 * 
 * @param[in] mlx The MLX instance handle. 
 * @param[in] key A specific mouse key. e.g MLX_MOUSE_BUTTON_0
 * @returns True or false if the mouse key is down or not.
 */
bool mlx_is_mouse_down(mlx_t* mlx, mouse_key_t key);

/**
 * Returns the current, relative, mouse cursor position on the window, starting
 * from the top left corner.
 * 
 * Negative values or values greater than window width or height 
 * indicate that it is outside the window.
 * 
 * @param[in] mlx The MLX instance handle. 
 * @param[in] pos_out The position.
 */
void mlx_get_mouse_pos(mlx_t* mlx, int32_t* x_out, int32_t* y_out);

/**
 * Sets the mouse position.
 * 
 * @param[in] mlx The MLX instance handle. 
 * @param[in] pos The position.
 */
void mlx_set_mouse_pos(mlx_t* mlx, int32_t x, int32_t y);

/**
 * Defines the state for the cursor, which can be:
 * - Normal
 * - Hidden
 * - Disabled
 * 
 * @param[in] mlx The MLX instance handle. 
 * @param[in] mode A specified mouse mode.
 */
void mlx_set_cursor_mode(mlx_t* mlx, mouse_mode_t mode);

/**
 * Allows for the creation of custom cursors with a given
 * XPM image.
 * 
 * Use mlx_set_cursor to select the specific cursor.
 * Cursors are destroyed at mlx_terminate().
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] image The XPM image to use as cursor.
 * @returns The cursor pointer.
 */
void* mlx_create_cursor(mlx_t* mlx, mlx_texture_t* image);

/**
 * Sets the current cursor to the given custom cursor.
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] cursor The cursor to display.
 */
void mlx_set_cursor(mlx_t* mlx, void* cursor);

//= Hooks =//

/**
 * This function sets the scroll callback, which is called when a scrolling 
 * device is used, such as a mouse wheel.
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] func The scroll wheel callback function.
 * @param[in] param An additional optional parameter.
 */
void mlx_scroll_hook(mlx_t* mlx, mlx_scrollfunc func, void* param);

/**
 * This function sets the key callback, which is called when a key is pressed
 * on the keyboard. Useful for single key press detection.
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] func The key press callback function.
 * @param[in] param An additional optional parameter.
 */
void mlx_key_hook(mlx_t* mlx, mlx_keyfunc func, void* param);

/**
 * This function sets the close callback, which is called in attempt to close 
 * the window device such as a close window widget used in the window bar.
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] func The close callback function.
 * @param[in] param An additional optional parameter.
 */
void mlx_close_hook(mlx_t* mlx, mlx_closefunc func, void* param);

/**
 * This function sets the resize callback, which is called when the window is
 * resized
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] func The resize callback function.
 * @param[in] param An additional optional parameter.
 */
void mlx_resize_hook(mlx_t* mlx, mlx_resizefunc func, void* param);

/**
 * Generic loop hook for any custom hooks to add to the main loop. 
 * Executes a function per frame, so be careful.
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] f The function.
 * @param[in] param The parameter to pass onto the function.
 * @returns Wether the hook was added successfuly. 
 */
bool mlx_loop_hook(mlx_t* mlx, void (*f)(void*), void* param);

//= Texture Functions =//

/**
 * Decode/load a PNG file onto a buffer. BPP will always be 4.
 * 
 * NOTE: The output type is just a t_mlx_texture *, it only has this typedef
 * because of norme.
 * 
 * @param[in] path Path to the PNG file.
 * @return If successful the texture data is returned, else NULL.
 */
mlx_texture_t* mlx_load_png(const char* path);

/**
 * Loads an XPM42 texture from the given file path.
 * 
 * @param[in] path The file path to the XPM image.
 * @returns The XPM texture struct containing its information.
 */
xpm_t* mlx_load_xpm42(const char* path);

/**
 * Deletes a texture by freeing its allocated data.
 * 
 * @param[in] texture The texture to free. 
 */
void mlx_delete_texture(mlx_texture_t* texture);

/**
 * Deletes an XPM42 texture by freeing its allocated data.
 * 
 * This will not remove any already drawn XPMs, it simply
 * deletes the XPM buffer.
 * 
 * @param xpm[in] The xpm texture to delete.
 */
void mlx_delete_xpm42(xpm_t* xpm);

/**
 * Converts a given texture to an image.
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] texture The texture to use to create the image from.
 * @return t_mlx_image* 
 */
mlx_image_t* mlx_texture_to_image(mlx_t* mlx, mlx_texture_t* texture);

/**
 * Given an X & Y coordinate and a Width and Height from a section of a
 * texture a new image is created, useful for texture atlases.
 * 
 * Basically a cropping tool.
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] texture The texture to use to create the image from.
 * @param[in] xy The X & Y location.
 * @param[in] wh The Width & Height.
 * @return t_mlx_image* 
 */
mlx_image_t* mlx_texture_area_to_image(mlx_t* mlx, mlx_texture_t* texture, int32_t xy[2], uint32_t wh[2]);

/**
 * Draws the texture on an already existing image.
 * 
 * @param[in] image The image to draw on.
 * @param[in] texture The texture to use to draw on the image.
 * @param[in] x X position relative to the image.
 * @param[in] y Y position relative to the image.
 * @return In-case of any issues, false else true.
 */
bool mlx_draw_texture(mlx_image_t* image, mlx_texture_t* texture, int32_t x, int32_t y);

//= Image Functions =//

/**
 * Sets / puts a pixel onto an image.
 * 
 * NOTE: It is considered undefined behaviour when putting a pixel 
 * beyond the bounds of an image.
 * 
 * @param[in] image The MLX instance handle.
 * @param[in] x The X coordinate position.
 * @param[in] y The Y coordinate position.
 * @param[in] color The color value to put.
 */
void mlx_put_pixel(mlx_image_t* image, int32_t x, int32_t y, uint32_t color);

/**
 * Creates and allocates a new image buffer.
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] width The desired width of the image.
 * @param[in] height The desired height of the image.
 * @return Pointer to the image buffer, if it failed to allocate then NULL.
 */
mlx_image_t* mlx_new_image(mlx_t* mlx, uint32_t width, uint32_t height);

/**
 * Draws a new instance of an image, it will then share the same
 * pixel buffer as the image.
 * 
 * NOTE: Keep in mind that the instance array gets reallocated, try to
 * to store the return value to the instance! 
 * NOT the pointer! It will become invalid!
 * 
 * WARNING: Try to display as few images onto the window as possible,
 * drawing too many images will cause a loss in peformance!
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] img The image to draw onto the screen.
 * @param[in] x The X position.
 * @param[in] y The Y position.
 * @return Index to the instance, or -1 on failure.
 */
int32_t mlx_image_to_window(mlx_t* mlx, mlx_image_t* img, int32_t x, int32_t y);

/**
 * Deleting an image will remove it from the render queue as well as any and all
 * instances it might have. Additionally, just as extra measures sets all the
 * data to NULL.
 * 
 * If you simply wish to stop rendering an image without de-allocation
 * set the 'enabled' boolean in the image struct.
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] image The image to delete.
 */
void mlx_delete_image(mlx_t* mlx, mlx_image_t* image);

/**
 * Allows you to resize an image to a new size, the pixel buffer is re-allocated
 * to fit & the previous data is copied over. New pixels are zeroed.
 * 
 * @param[in] img The image to resize.
 * @param[in] nwidth The new width.
 * @param[in] nheight The new height.
 * @return True if image was resize or false on error.
 */
bool mlx_resize_image(mlx_image_t* img, uint32_t nwidth, uint32_t nheight);

/**
 * Sets the depth / Z axis value of an instance.
 * 
 * NOTE: Keep in mind that images that are on the same Z layer, cut each other off.
 * so if you don't see your image anymore make sure its not conflicting by being on
 * the same layer as another image.
 * 
 * @param instance The instane on which to change the depth.
 * @param zdepth The new depth value.
 */
void mlx_set_instance_depth(mlx_instance_t* instance, int32_t zdepth);

//= String Functions =//

/**
 * Draws a string onto an image and then outputs it onto the window.
 * 
 * @param[in] mlx The MLX instance handle.
 * @param[in] str The string to draw.
 * @param[in] x The X location.
 * @param[in] y The Y location.
 * @return Image ptr to the string.
 */
mlx_image_t* mlx_put_string(mlx_t* mlx, const char* str, int32_t x, int32_t y);

#endif
